#include "globals/scheduler.h"
#include "scheduler.h"
#include "nexus.h"
#include "value.h"
#include "task.h"

#include <boost/thread.hpp>

const JSClassDefinition NX::Globals::Scheduler::Class {
  0, kJSClassAttributeNone, "Scheduler", nullptr, NX::Globals::Scheduler::Properties, NX::Globals::Scheduler::Methods
};


JSValueRef NX::Globals::Scheduler::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Nexus * nx = reinterpret_cast<NX::Nexus*>(JSObjectGetPrivate(object));
  return JSObjectMake(nx->context(), nx->defineOrGetClass(NX::Globals::Scheduler::Class), nx->scheduler().get());
}

const JSStaticValue NX::Globals::Scheduler::Properties[] {
  { "threadId", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    std::ostringstream ss;
    ss << boost::this_thread::get_id();
    std::string id = ss.str();
    return NX::Value(ctx, id).value();
  }, nullptr, kJSPropertyAttributeReadOnly
  },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Scheduler::Methods[] {
  { "schedule", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Scheduler * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(thisObject));
      NX::Nexus * nx = scheduler->nexus();
      JSObjectRef fun = argumentCount > 0 ? JSValueToObject(ctx, arguments[0], exception) : nullptr;
      if (fun) {
        JSValueProtect(nx->threadLocalContext(), fun);
        NX::AbstractTask * taskPtr = scheduler->scheduleTask([=]() {
          JSValueRef exp = nullptr;
          JSValueRef ret = JSObjectCallAsFunction(nx->threadLocalContext(), fun, nullptr, 0, nullptr, &exp);
          if (exp) {
            NX::Nexus::ReportException(nx->threadLocalContext(), exp);
          }
          JSValueUnprotect(nx->threadLocalContext(), fun);
        });
        if (!taskPtr) {
          NX::Value message(ctx, "unable to schedule task");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        } else {
          JSStringRef abortName = JSStringCreateWithUTF8CString("abort");
          JSObjectRef taskObject = JSObjectMake(nx->threadLocalContext(), nx->defineOrGetClass({ 0, 0, "Task" }), taskPtr);
          JSObjectSetProperty(ctx, taskObject, abortName, JSObjectMakeFunctionWithCallback(
            ctx, abortName, [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                               size_t argumentCount, const JSValueRef arguments[],
                               JSValueRef* exception) -> JSValueRef
                               {
                                 NX::AbstractTask * task = reinterpret_cast<NX::AbstractTask *>(JSObjectGetPrivate(thisObject));
                                 if (task) {
                                   if (task->status() == NX::AbstractTask::ABORTED)
                                     return JSValueMakeBoolean(ctx, false);
                                   else {
                                     task->abort();
                                     return JSValueMakeBoolean(ctx, task->status() == NX::AbstractTask::ABORTED);
                                   }
                                 } else {
                                   NX::Value message(ctx, "abort called on a non-task");
                                   JSValueRef args[] { message.value(), nullptr };
                                   *exception = JSObjectMakeError(ctx, 1, args, nullptr);
                                 }
                                 return JSValueMakeUndefined(ctx);
                               }
          ), kJSPropertyAttributeNone, nullptr);
          JSStringRelease(abortName);
          return taskObject;
        }
      } else {
        if (!*exception) {
          NX::Value message(ctx, "invalid argument passed to scheduler");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        }
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  //     { "yield", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
  //       size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
  //         NX::Scheduler * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(thisObject));
  //         JSGlobalContextRef globalContext = JSContextGetGlobalContext(ctx);
  //         JSValueProtect(globalContext, function);
  //         JSValueProtect(globalContext, thisObject);
  //         try {
  //           scheduler->yield();
  //         } catch(const std::exception & e) {
  //           NX::Value message(ctx, e.what());
  //           JSValueRef args[] { message.value(), nullptr };
  //           *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  //         }
  //         JSValueUnprotect(globalContext, function);
  //         JSValueUnprotect(globalContext, thisObject);
  //         return JSValueMakeUndefined(ctx);
  //       }, 0
  //     },
  { nullptr, nullptr, 0 }
};

