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

#ifndef CLASSES_TASK_H
#define CLASSES_TASK_H

#include <JavaScript.h>
#include <memory>

#include "nexus.h"
#include "classes/emitter.h"

namespace NX {
  class Context;
  class AbstractTask;
  namespace Classes {
    class Task: public virtual NX::Classes::Emitter
    {
    private:
      static const JSClassDefinition Class;
      static const JSStaticValue Properties[];
      static const JSStaticFunction Methods[];

      static void Finalize(JSObjectRef object) {
        delete FromObject(object);
      }

      static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                     const JSValueRef arguments[], JSValueRef* exception);

    public:
      static JSClassRef createClass(NX::Context * context);
      static JSObjectRef getConstructor(NX::Context * context);
      static JSObjectRef wrapTask(JSContextRef ctx, NX::AbstractTask * task);

      static NX::Classes::Task * FromObject(JSObjectRef object) {
        return dynamic_cast<NX::Classes::Task *>(NX::Classes::Base::FromObject(object));
      }

    public:
      Task(NX::AbstractTask * task): myTask(task) { };
      virtual ~Task() {}

      NX::AbstractTask * task() { return myTask; }

      virtual NX::AbstractTask::Status status() {
        if (NX::AbstractTask * task = myTask)
          return task->status();
        else
          return NX::AbstractTask::UNKNOWN;
      }

    private:
      std::atomic<NX::AbstractTask *> myTask;
    };
  }
}

#endif // CLASSES_TASK_H
