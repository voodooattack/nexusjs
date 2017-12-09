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

#ifndef TASK_H
#define TASK_H

#include <boost/noncopyable.hpp>
#include <boost/coroutine2/all.hpp>
#include <utility>

#include "scheduler.h"
#include "exception.h"

#include <wtf/FastMalloc.h>

namespace NX
{
  class Task;
  class CoroutineTask;
  class AbstractTask: public boost::noncopyable {

    WTF_MAKE_FAST_ALLOCATED;

    friend class NX::Scheduler;
    friend class boost::thread_specific_ptr<AbstractTask>;
    friend class Task;
    friend class CoroutineTask;
  protected:

    NX::Scheduler::Holder myHolder;

    AbstractTask(NX::Scheduler * scheduler, bool hold): myHolder(hold ? scheduler : nullptr) {}

    virtual ~AbstractTask() = default;

  public:

    enum Status {
      INACTIVE,
      CREATED,
      ACTIVE,
      PENDING,
      FINISHED,
      ABORTED,
      UNKNOWN
    };

    virtual Scheduler * scheduler() = 0;
    virtual Status status() const = 0;
    virtual void abort() = 0;
    virtual void create() = 0;
    virtual void enter() = 0;
    virtual void yield() = 0;
    virtual void exit() = 0;
    virtual void addCancellationHandler(const NX::Scheduler::CompletionHandler &) = 0;
    virtual void addCompletionHandler(const NX::Scheduler::CompletionHandler &) = 0;

    virtual void await() = 0;

  };

  class Task: public AbstractTask {
  protected:
    ~Task() override { }

    WTF_MAKE_FAST_ALLOCATED;

  public:
    Task(NX::Scheduler::CompletionHandler && handler, NX::Scheduler * scheduler, bool hold = true):
      AbstractTask(scheduler, hold), myHandler(std::move(handler)), myScheduler(scheduler), myStatus(INACTIVE)
    {
    }

    Scheduler * scheduler() override { return myScheduler; }
    Status status() const override { return myStatus; }
    void abort() override { myStatus.store(ABORTED); for (auto & i : myCancellationHandlers) i(); }
    void create() override { myStatus.store(CREATED); }
    void enter() override;
    void yield() override { throw NX::Exception("can't yield on a regular task"); }
    void exit() override { for(auto & i : myCompletionHandlers) i(); }
    void addCancellationHandler(const NX::Scheduler::CompletionHandler & handler) override {
      myCancellationHandlers.push_back(handler);
    }
    void addCompletionHandler(const NX::Scheduler::CompletionHandler & handler) override {
      myCompletionHandlers.push_back(handler);
    }

    void await() override {
      if (myStatus == Status::FINISHED || myStatus == Status::ABORTED)
        return;
      std::atomic_bool finished(false);
      myCancellationHandlers.emplace_back([&] { finished.store(true); });
      myCompletionHandlers.emplace_back([&] { finished.store(true); });
      while (!finished)
        myScheduler->yield();
    }

  protected:
    NX::Scheduler::CompletionHandler myHandler;
    std::vector<NX::Scheduler::CompletionHandler> myCancellationHandlers, myCompletionHandlers;
    NX::Scheduler * myScheduler;
    boost::atomic<Status> myStatus;
  };

  class CoroutineTask: public AbstractTask {
    typedef boost::coroutines2::coroutine<void> coro_t;
    typedef coro_t::push_type push_type;
    typedef boost::coroutines2::coroutine<void>::pull_type pull_type;

  WTF_MAKE_FAST_ALLOCATED;

  protected:
    ~CoroutineTask() override = default;

  public:
    CoroutineTask(NX::Scheduler::CompletionHandler &&, NX::Scheduler *, bool hold = false);

    Scheduler * scheduler() override { return myScheduler; }

    Status status() const override { return myStatus; }

    void abort() override { myStatus.store(ABORTED); for (auto & i : myCancellationHandlers) i(); }
    void create() override;
    void enter() override;
    void yield() override;
    void exit() override { for (auto & i: myCompletionHandlers) i(); }

    void addCancellationHandler(const NX::Scheduler::CompletionHandler & handler) override {
      myCancellationHandlers.push_back(handler);
    }
    void addCompletionHandler(const NX::Scheduler::CompletionHandler & handler) override {
      myCompletionHandlers.push_back(handler);
    }

    void await() override {
      if (myStatus == Status::FINISHED || myStatus == Status::ABORTED)
        return;
      std::atomic_bool finished(false);
      myCancellationHandlers.emplace_back([&] { finished.store(true); });
      myCompletionHandlers.emplace_back([&] { finished.store(true); });
      while (!finished)
        myScheduler->yield();
    }

  protected:
    void coroutine(pull_type & ca);
  protected:
    NX::Scheduler::CompletionHandler myHandler;
    std::vector<NX::Scheduler::CompletionHandler> myCancellationHandlers, myCompletionHandlers;
    NX::Scheduler * myScheduler;
    std::shared_ptr<push_type> myCoroutine;
    pull_type * myPullCa;
    boost::atomic<Status> myStatus;
  };

  class TaskGroup: public std::vector<NX::AbstractTask*> {
  public:
    explicit TaskGroup(NX::Scheduler * scheduler): std::vector<NX::AbstractTask*>(), myScheduler(scheduler),
                                                   myFinished(nullptr) { attach(); }
    TaskGroup(std::vector<NX::AbstractTask*> tasks, NX::Scheduler * scheduler):
      std::vector<NX::AbstractTask*>(std::move(tasks)), myScheduler(scheduler), myFinished(nullptr) { attach(); }
    TaskGroup(std::vector<NX::AbstractTask*> && tasks, NX::Scheduler * scheduler):
      std::vector<NX::AbstractTask*>(tasks), myScheduler(scheduler), myFinished(nullptr) { attach(); }

    TaskGroup(NX::TaskGroup && other):
      std::vector<NX::AbstractTask*>(other), myScheduler(other.myScheduler), myFinished(other.myFinished)
    {
    }

    Scheduler * scheduler() { return myScheduler; }

    void push_back(NX::AbstractTask* task) {
      attach(task);
      std::vector<NX::AbstractTask*>::push_back(task);
    }

    void emplace_back(NX::AbstractTask* task) {
      attach(task);
      std::vector<NX::AbstractTask*>::emplace_back(task);
    }

    void attach() {
      myFinished = std::make_shared<std::atomic_size_t>(0);
      for (auto task : *this) {
        attach(task);
      }
    }

    void attach(NX::AbstractTask * task) {
      auto status = task->status();
      if (status == NX::AbstractTask::Status::FINISHED || status == NX::AbstractTask::Status::ABORTED)
        (*myFinished)++;
      else {
        std::shared_ptr<std::atomic_size_t> finished(myFinished);
        task->addCompletionHandler([finished] { (*finished)++; });
        task->addCancellationHandler([finished] { (*finished)++; });
      }
    }

    void await() {
      while (*myFinished < size())
        myScheduler->yield();
    }

  private:
    NX::Scheduler * myScheduler;
    std::shared_ptr<std::atomic_size_t> myFinished;
  };
}

#endif // TASK_H
