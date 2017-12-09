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

#include "classes/task.h"
#include "task.h"

JSClassRef NX::Classes::Task::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::Task::Class;
  def.parentClass = NX::Classes::Emitter::createClass(context);
  return context->nexus()->defineOrGetClass (def);
}

JSObjectRef NX::Classes::Task::Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef emitterClass = createClass(context);
  try {
    NX::Object fun(context->toJSContext(), arguments[0]);
    JSObjectRef thisObject = nullptr;
    thisObject = JSObjectMake(ctx, emitterClass, dynamic_cast<NX::Classes::Base*>(new NX::Classes::Task(new NX::Task([=](){
      JSValueRef exp = nullptr;
      JSObjectCallAsFunction(context->toJSContext(), fun, nullptr, 0, nullptr, &exp);
      if (exp) {
        NX::Nexus::ReportException(context->toJSContext(), exp);
      }
      JSValueUnprotect(context->toJSContext(), thisObject);
    }, context->nexus()->scheduler()))));
    JSValueProtect(context->toJSContext(), thisObject);
    return thisObject;
  } catch (const std::exception & e) {
    JSWrapException(ctx, e, exception);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
}

JSObjectRef NX::Classes::Task::getConstructor(NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::Task::Constructor);
}

JSObjectRef NX::Classes::Task::wrapTask (JSContextRef ctx, NX::AbstractTask * task)
{
  NX::Context * context = NX::Context::FromJsContext (ctx);
  auto * wrapper = new Task (task);
  JSObjectRef ret = JSObjectMake (context->toJSContext(), createClass (context), wrapper);
  JSValueProtect(context->toJSContext(), ret);
  auto handler = [=]() { wrapper->myTask.store(nullptr); JSValueUnprotect(context->toJSContext(), ret); };
  auto completionHandler = [=]{
    if (dynamic_cast<NX::CoroutineTask*>(task)) {
      context->nexus()->scheduler()->scheduleTask([=] {
        wrapper->emitFast(context->toJSContext(), ret, "completed", 0, nullptr, nullptr);
        handler();
      });
    } else {
      wrapper->emitFast(context->toJSContext(), ret, "completed", 0, nullptr, nullptr);
      handler();
    }
  };
  auto cancellationHandler = [=]{
    if (dynamic_cast<NX::CoroutineTask*>(task)) {
      context->nexus()->scheduler()->scheduleTask([=] {
        wrapper->emitFast(context->toJSContext(), ret, "aborted", 0, nullptr, nullptr);
        handler();
      });
    } else {
      wrapper->emitFast(context->toJSContext(), ret, "aborted", 0, nullptr, nullptr);
      handler();
    }
  };
  task->addCompletionHandler(completionHandler);
  task->addCancellationHandler(cancellationHandler);
  return ret;
}

const JSClassDefinition NX::Classes::Task::Class {
  0, kJSClassAttributeNone, "Task", nullptr, NX::Classes::Task::Properties,
  NX::Classes::Task::Methods, nullptr, NX::Classes::Task::Finalize
};

const JSStaticValue NX::Classes::Task::Properties[] {
  { "status", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef
    {
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      NX::Classes::Task * task = FromObject(object);
      if (!task) {
        NX::Value message(ctx, "invalid `this` value");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      NX::AbstractTask::Status status = task->status();
      switch(status) {
        case NX::AbstractTask::CREATED:
          return NX::Value(ctx, "created").value();
        case NX::AbstractTask::ABORTED:
          return NX::Value(ctx, "aborted").value();
        case NX::AbstractTask::ACTIVE:
          return NX::Value(ctx, "active").value();
        case AbstractTask::INACTIVE:
          return NX::Value(ctx, "inactive").value();
        case AbstractTask::FINISHED:
          return NX::Value(ctx, "finished").value();
        case AbstractTask::PENDING:
          return NX::Value(ctx, "pending").value();
        default:
          return NX::Value(ctx, "unknown").value();
      }
    }, nullptr, kJSPropertyAttributeReadOnly
  },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Task::Methods[] {
  { "abort", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef
    {
      try {
//        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::Task * task = NX::Classes::Task::FromObject(thisObject);
        if (NX::AbstractTask * realTask = task->task()) {
          realTask->abort();
          return JSValueMakeBoolean(ctx, true);
        } else {
          return JSValueMakeBoolean(ctx, false);
        }
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};



