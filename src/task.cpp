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

#include "task.h"
#include "scheduler.h"

#include <iostream>

NX::CoroutineTask::CoroutineTask (NX::Scheduler::CompletionHandler handler, NX::Scheduler * scheduler):
  myHandler(handler), myScheduler(scheduler), myCoroutine(), myPullCa(nullptr), myStatus(INACTIVE)
{

}

void NX::CoroutineTask::create()
{
  if (myStatus == ABORTED) return;
  myStatus.store(CREATED);
  myScheduler->makeCurrent(this);
  myCoroutine.reset(new NX::CoroutineTask::push_type(boost::bind(&NX::CoroutineTask::coroutine, this, _1)));
}

void NX::CoroutineTask::enter()
{
  if (myStatus == ABORTED) return;
  myStatus.store(ACTIVE);
  myScheduler->makeCurrent(this);
  myCoroutine->operator()();
}

void NX::CoroutineTask::yield()
{
  if (myStatus == ABORTED)
    throw std::runtime_error("task aborted, could not yield");
  myStatus.store(PENDING);
  myPullCa->operator()();
  myScheduler->makeCurrent(this);
  myStatus.store(ACTIVE);
}

void NX::CoroutineTask::coroutine (NX::CoroutineTask::pull_type & ca)
{
  if (myStatus == ABORTED) return;
  this->myPullCa = &ca;
  myScheduler->makeCurrent(this);
  myStatus.store(ACTIVE);
  myHandler();
  myStatus.store(FINISHED);
}



