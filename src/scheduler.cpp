/*
 * Nexus.js - The next-gen JavaScript platform
 * Copyright (C) 2016  Abdullah A. Hassan <abdullah@webtomizer.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nexus.h"
#include "scheduler.h"
#include "task.h"

#include <functional>
#include <JavaScriptCore/runtime/InitializeThreading.h>
#include <JavaScriptCore/heap/GCDeferralContext.h>
#include <JavaScriptCore/heap/GCDeferralContextInlines.h>
#include <JavaScriptCore/heap/Heap.h>
#include <JavaScriptCore/heap/HeapInlines.h>
#include <JavaScriptCore/heap/MachineStackMarker.h>

NX::Scheduler::Scheduler (NX::Nexus * nexus, unsigned int maxThreads):
  myNexus(nexus), myMaxThreads(maxThreads), myThreadCount(0), myService(), myWork(),
  myThreadGroup(), myCurrentTask(nullptr), myTaskQueue(256), myTaskCount(0), myActiveTaskCount(0), myHoldCount(0),
  myPauseTasks(false)
{
  myService.reset(new boost::asio::io_service(maxThreads));
  myService->stop();
}

NX::Scheduler::~Scheduler()
{
  this->stop();
  myThreadGroup.join_all();
  for(auto task : myThreadInitQueue) {
    delete task;
  }
  myThreadInitQueue.clear();
}

void NX::Scheduler::addThread()
{
  myThreadGroup.add_thread(new boost::thread(boost::bind(&NX::Scheduler::dispatcher, this)));
}

void NX::Scheduler::dispatcher()
{
  static const std::chrono::microseconds delay(200);
  for(auto task : myThreadInitQueue) {
    if (task->status() != NX::AbstractTask::Status::ABORTED) {
      task->create();
      task->enter();
      task->exit();
    }
  }
  myThreadCount++;
  while (myService->poll_one() || queued())
  {
    if (!drainTasks()) {
      std::this_thread::sleep_for(delay);
    }
    std::this_thread::yield();
    if (myService->stopped())
      break;
  }
  myThreadCount--;
}

void NX::Scheduler::makeCurrent (NX::AbstractTask * task)
{
  myCurrentTask.reset(task);
}

std::size_t NX::Scheduler::drainTasks()
{
  if (myPauseTasks) return 0;
  std::size_t processed = 0;
  NX::AbstractTask * task = nullptr;
  while (myTaskQueue.pop(task))
  {
    myActiveTaskCount++;
    myTaskCount--;
    myCurrentTask.reset(task);
    if (myCurrentTask->status() != NX::AbstractTask::ABORTED) {
      if (myCurrentTask.get() && myCurrentTask->status() == NX::AbstractTask::INACTIVE)
        myCurrentTask->create();
      if (myCurrentTask.get() &&(myCurrentTask->status() == NX::AbstractTask::CREATED ||
                                 myCurrentTask->status() == NX::AbstractTask::PENDING)) {
        myCurrentTask->enter();
      }
      if (myCurrentTask.get() && myCurrentTask->status() == NX::AbstractTask::PENDING)
      {
        myTaskQueue.push(myCurrentTask.release());
      }
      else if (auto pTask = myCurrentTask.release()) {
        pTask->exit();
        delete pTask;
      }
    }
    myActiveTaskCount--;
    processed++;
    if (myPauseTasks) break;
  }
  return processed;
}

void NX::Scheduler::balanceThreads()
{
  while(myTaskCount > myThreadGroup.size() && myThreadGroup.size() < myMaxThreads)
    addThread();
}

void NX::Scheduler::start()
{
  BOOST_ASSERT_MSG(myService->stopped(), "call to start with a service already running");
  myService->reset();
  myWork.reset(new boost::asio::io_service::work(*myService));
  balanceThreads();
}

void NX::Scheduler::stop()
{
  myWork.reset();
  myService->stop();
}

void NX::Scheduler::join()
{
  myThreadGroup.join_all();
}

NX::AbstractTask * NX::Scheduler::scheduleAbstractTask (NX::AbstractTask * task)
{
  myTaskCount++;
  myTaskQueue.push(task);
  balanceThreads();
  return task;
}

void NX::Scheduler::yield()
{
  if (!myCurrentTask.get()) {
    throw NX::Exception("call to yield outside of a scheduler task");
  }
  if (!dynamic_cast<NX::CoroutineTask*>(myCurrentTask.get())) {
    throw NX::Exception("call to yield outside of a coroutine");
  }
  myCurrentTask->yield();
}

void NX::Scheduler::joinPool(const CompletionHandler & drainTasks) {
  do {
    dispatcher();
    if (drainTasks)
      drainTasks();
    std::this_thread::sleep_for(std::chrono::microseconds(200));
  } while (myHoldCount);
}

NX::Task *NX::Scheduler::scheduleTask(CompletionHandler &&handler) {
  if (!handler)
    throw NX::Exception("empty handler provided");
  auto * task = new NX::Task(std::move(handler), this);
  scheduleAbstractTask(task);
  return task;
}

NX::Task *NX::Scheduler::scheduleTask(const NX::Scheduler::duration &time, CompletionHandler && handler) {
  if (!handler)
    throw NX::Exception("empty handler provided");
  auto * taskObject = new NX::Task(std::move(handler), this);
  std::shared_ptr<timer_type> timer(new timer_type(*myService));
  timer->expires_from_now(time);
  timer->async_wait(boost::bind(boost::bind(&Scheduler::scheduleAbstractTask, this, taskObject), timer));
  taskObject->addCancellationHandler([=]() { timer->cancel(); });
  return taskObject;
}

NX::CoroutineTask *NX::Scheduler::scheduleCoroutine(CompletionHandler &&handler) {
  if (!handler)
    throw NX::Exception("empty handler provided");
  auto * task = new NX::CoroutineTask(std::move(handler), this);
  scheduleAbstractTask(task);
  return task;
}

NX::CoroutineTask *NX::Scheduler::scheduleCoroutine(const NX::Scheduler::duration &time, CompletionHandler &&handler) {
  if (!handler)
    throw NX::Exception("empty handler provided");
  auto * taskObject = new NX::CoroutineTask(std::move(handler), this);
  std::shared_ptr<timer_type> timer(new timer_type(*myService));
  timer->expires_from_now(time);
  timer->async_wait(boost::bind(boost::bind(&Scheduler::scheduleAbstractTask, this, taskObject), timer));
  taskObject->addCancellationHandler([=]() { timer->cancel(); });
  return taskObject;
}

bool NX::Scheduler::canYield() const {
  return dynamic_cast<NX::CoroutineTask*>(myCurrentTask.get()) != nullptr;
}

NX::Task *NX::Scheduler::scheduleThreadInitTask(NX::Scheduler::CompletionHandler &&handler) {
  if (!handler)
    throw NX::Exception("empty handler provided");
  auto task = new NX::Task(std::move(handler), this, false);
  myThreadInitQueue.emplace_back(task);
  return task;
}

NX::Scheduler::Holder::Holder() : myScheduler(nullptr) {}

NX::Scheduler::Holder::Holder(NX::Scheduler *scheduler) : myScheduler(scheduler) {
  if (myScheduler)
    myScheduler->hold();
}

NX::Scheduler::Holder::Holder(const NX::Scheduler::Holder &other) : myScheduler(other.myScheduler) {
  if (myScheduler)
    myScheduler->hold();
}

NX::Scheduler::Holder::~Holder() { if (myScheduler) myScheduler->release(); }

void NX::Scheduler::Holder::reset() {
  if (myScheduler)
    myScheduler->release();
  myScheduler = nullptr;
}
