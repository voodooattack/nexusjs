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
#include <boost/asio/handler_type.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>

#include <thread>

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
    typedef boost::function<void(void)> CompletionHandler;
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
    void joinPool() { dispatcher(); }

    NX::AbstractTask * scheduleAbstractTask(NX::AbstractTask * task);

    NX::Task * scheduleTask(CompletionHandler handler);
    NX::Task * scheduleTask(const duration & time, CompletionHandler handler);

    NX::CoroutineTask * scheduleCoroutine(CompletionHandler handler);
    NX::CoroutineTask * scheduleCoroutine(const duration & time, CompletionHandler handler);

    void yield();

    NX::Nexus * nexus() { return myNexus; }

    unsigned int concurrency() const { return myThreadCount; }
    unsigned int queued() const { return myTaskCount; }
    unsigned int active() const { return myActiveTaskCount; }
    unsigned int remaining() const { return myActiveTaskCount + myTaskCount; }

    const std::shared_ptr<boost::asio::io_service> service() const { return myService; }
    std::shared_ptr<boost::asio::io_service> service() { return myService; }

    /**
     * Internal use only!
     */
    void makeCurrent(NX::AbstractTask * task);

  protected:
    void addThread();
    void balanceThreads();
    void dispatcher();
    bool processTasks();
  private:
    NX::Nexus * myNexus;
    boost::atomic<unsigned int> myMaxThreads;
    boost::atomic_uint myThreadCount;
    std::shared_ptr<boost::asio::io_service> myService;
    std::shared_ptr<boost::asio::io_service::work> myWork;
    std::vector<std::thread> myThreadGroup;
    boost::thread_specific_ptr<NX::AbstractTask> myCurrentTask;
    TaskQueue myTaskQueue;
    boost::atomic_uint myTaskCount, myActiveTaskCount;
    boost::atomic_bool myPauseTasks;
  };
}
#endif // SCHEDULER_H
