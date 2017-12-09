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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#undef B0 // JavaSCriptCore will complain if this is defined.
#include <boost/asio/handler_type.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <thread>
#include <WTF/wtf/ThreadGroup.h>
#include <WTF/wtf/PriorityQueue.h>
#include "exception.h"

namespace NX
{
  class Nexus;
  class AbstractTask;
  class Task;
  class CoroutineTask;
  class Scheduler: public boost::noncopyable
  {
  public:
    typedef boost::asio::deadline_timer timer_type;
    typedef timer_type::duration_type duration;
    typedef std::function<void(void)> CompletionHandler;
    typedef boost::lockfree::queue<NX::AbstractTask*> TaskQueue;
  public:
    Scheduler(NX::Nexus *, unsigned int maxThreads);
    virtual ~Scheduler();
  public:
    void start();
    void pause() { myPauseTasks.store(true); }
    void resume() { myPauseTasks.store(false); }
    void stop();
    void join();
    void joinPool();

    NX::AbstractTask * scheduleAbstractTask(NX::AbstractTask * task);

    NX::Task * scheduleTask(CompletionHandler && handler);
    NX::Task * scheduleTask(const duration & time, CompletionHandler && handler);
    NX::CoroutineTask * scheduleCoroutine(CompletionHandler && handler);
    NX::CoroutineTask * scheduleCoroutine(const duration & time, CompletionHandler && handler);

    NX::Task * scheduleThreadInitTask(CompletionHandler && handler);

    void yield();

    NX::Nexus * nexus() { return myNexus; }

    std::size_t concurrency() const { return myThreadCount; }
    std::size_t queued() const { return myTaskCount; }
    std::size_t active() const { return myActiveTaskCount; }
    std::size_t remaining() const { return (std::size_t)myTaskCount + (std::size_t)myActiveTaskCount; }

    struct Holder {
      Holder();
      Holder(const Holder& other);
      explicit Holder(Scheduler * scheduler);
      ~Holder();

      void reset();

    private:
      Scheduler * myScheduler;
    };

    void hold() { myHoldCount++; }
    void release() { myHoldCount--; }

    const std::shared_ptr<boost::asio::io_service> service() const { return myService; }
    std::shared_ptr<boost::asio::io_service> service() { return myService; }

    /**
     * Internal use only!
     */
    void makeCurrent(NX::AbstractTask * task);

    bool canYield() const;

  protected:
    void addThread();
    void balanceThreads();
    void dispatcher();
    std::size_t drainTasks();
  private:
    NX::Nexus * myNexus;
    std::atomic_size_t myMaxThreads;
    std::atomic_size_t myThreadCount;
    std::shared_ptr<boost::asio::io_service> myService;
    std::shared_ptr<boost::asio::io_service::work> myWork;
    boost::thread_group myThreadGroup;
    boost::thread_specific_ptr<NX::AbstractTask> myCurrentTask;
    TaskQueue myTaskQueue;
    std::vector<NX::AbstractTask*> myThreadInitQueue;
    std::atomic_size_t myTaskCount, myActiveTaskCount, myHoldCount;
    std::atomic_bool myPauseTasks;
  };
}
#endif // SCHEDULER_H
