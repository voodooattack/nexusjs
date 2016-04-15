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
#include <boost/coroutine/all.hpp>

#include "scheduler.h"

namespace NX
{
  class Task;
  class CoroutineTask;
  class AbstractTask: public boost::noncopyable {
    friend class NX::Scheduler;
    friend class boost::thread_specific_ptr<AbstractTask>;
    friend class Task;
    friend class CoroutineTask;
  protected:
    virtual ~AbstractTask() { }
  public:

    enum Status {
      INACTIVE,
      CREATED,
      ACTIVE,
      PENDING,
      FINISHED,
      ABORTED,
    };

    virtual Scheduler * scheduler() = 0;
    virtual Status status() const = 0;
    virtual void abort() = 0;
    virtual void create() = 0;
    virtual void enter() = 0;
    virtual void yield() = 0;
    virtual void exit() = 0;
    virtual void schedule(NX::AbstractTask * task) = 0;
    virtual void setCancellationHandler(const NX::Scheduler::CompletionHandler &) = 0;

  };

  class Task: public AbstractTask {
  protected:
    virtual ~Task() { }
  public:
    Task(NX::Scheduler::CompletionHandler handler, NX::Scheduler * scheduler):
      myHandler(handler), myScheduler(scheduler), myStatus(INACTIVE)
    {
    }
    virtual Scheduler * scheduler() { return myScheduler; }
    virtual Status status() const { return myStatus; }
    virtual void abort() { myStatus.store(ABORTED); if (myCancellationHandler) myCancellationHandler(); }
    virtual void create() { myStatus.store(CREATED); }
    virtual void enter() {
      if (myStatus == ABORTED) return;
      myStatus.store(ACTIVE);
      myScheduler->makeCurrent(this);
      myHandler();
      myStatus.store(FINISHED);
    }
    virtual void yield() { throw std::runtime_error("can't yield on a regular task"); }
    virtual void exit() {
      for(auto i : myDependents)
        if (status() != ABORTED)
          myScheduler->scheduleAbstractTask(i);
        else {
          i->abort();
          delete i;
        }
    }
    virtual void schedule(NX::AbstractTask * task) { myDependents.push_back(task); }
    virtual void setCancellationHandler(const NX::Scheduler::CompletionHandler & handler) {
      myCancellationHandler = handler;
    }
  protected:
    NX::Scheduler::CompletionHandler myHandler, myCancellationHandler;
    NX::Scheduler * myScheduler;
    boost::atomic<Status> myStatus;
    std::vector<NX::AbstractTask *> myDependents;
  };

  class CoroutineTask: public AbstractTask {
    typedef boost::coroutines::coroutine<void> coro_t;
    typedef boost::coroutines::coroutine<void>::push_type push_type;
    typedef boost::coroutines::coroutine<void>::pull_type pull_type;
  protected:
    virtual ~CoroutineTask() { }
  public:
    CoroutineTask(NX::Scheduler::CompletionHandler, NX::Scheduler *);
    virtual Scheduler * scheduler() { return myScheduler; }
    virtual Status status() const { return myStatus; }
    virtual void abort() { myStatus.store(ABORTED); if (myCancellationHandler) myCancellationHandler(); }
    virtual void create();
    virtual void enter();
    virtual void yield();
    virtual void exit() {
      for(auto i : myDependents)
        if (status() != ABORTED)
          myScheduler->scheduleAbstractTask(i);
        else {
          i->abort();
          delete i;
        }
    }
    virtual void schedule(NX::AbstractTask * task) { myDependents.push_back(task); }
    virtual void setCancellationHandler(const NX::Scheduler::CompletionHandler & handler) {
      myCancellationHandler = handler;
    }
  protected:
    void coroutine(pull_type & ca);
  protected:
    NX::Scheduler::CompletionHandler myHandler, myCancellationHandler;
    NX::Scheduler * myScheduler;
    boost::shared_ptr<push_type> myCoroutine;
    pull_type * myPullCa;
    boost::atomic<Status> myStatus;
    std::vector<NX::AbstractTask *> myDependents;
  };
}

#endif // TASK_H
