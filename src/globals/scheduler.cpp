#include "nexus.h"
#include "globals/scheduler.h"
#include "scheduler.h"
#include "value.h"
#include "task.h"
#include "classes/task.h"

#include <boost/thread.hpp>

const JSClassDefinition NX::Globals::Scheduler::Class {
  0, kJSClassAttributeNone, "Scheduler", nullptr, NX::Globals::Scheduler::Properties, NX::Globals::Scheduler::Methods
};


JSValueRef NX::Globals::Scheduler::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (auto Scheduler = context->getGlobal("Nexus.Scheduler"))
      return Scheduler;
  return context->setGlobal("Nexus.Scheduler", JSObjectMake(context->toJSContext(),
                                                     context->nexus()->defineOrGetClass(NX::Globals::Scheduler::Class),
                                                     context->nexus()->scheduler()));
}

const JSStaticValue NX::Globals::Scheduler::Properties[] {
  { "threadId", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      std::ostringstream ss;
      ss << boost::this_thread::get_id();
      std::string id = ss.str();
      return NX::Value(ctx, id).value();
    }, nullptr, kJSPropertyAttributeReadOnly
  },
  { "concurrency", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Scheduler * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(object));
      return NX::Value(ctx, scheduler->concurrency()).value();
    }, nullptr, kJSPropertyAttributeReadOnly
  },
  { "Task", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      if (JSObjectRef Task = JSValueToObject(ctx, context->getGlobal("Nexus.Scheduler.Task"), exception))
        if (!*exception)
          return Task;
      return context->setGlobal("Nexus.Scheduler.Task", NX::Classes::Task::getConstructor(context));
    }, nullptr, kJSPropertyAttributeReadOnly
  },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Scheduler::Methods[] {
  { "schedule", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      auto context = Context::FromJsContext(ctx);
      auto * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(thisObject));
      if (argumentCount != 1) {
        *exception = NX::Object(ctx, NX::Exception("Scheduler.schedule called without an argument"));
        return JSValueMakeUndefined(ctx);
      }
      if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject) {
        *exception = NX::Object(ctx, NX::Exception("invalid argument passed to Scheduler.schedule"));
        return JSValueMakeUndefined(ctx);
      }
      NX::Object fun(context->toJSContext(), arguments[0]);
      if (exception && *exception)
        return JSValueMakeUndefined(ctx);
      if (fun) {
        if (JSObjectIsFunction(ctx, fun)) {
          NX::AbstractTask * taskPtr = new NX::Task([=]() {
            JSValueRef exp = nullptr;
            fun.call(nullptr, std::vector<JSValueRef>(), &exp);
            if (exp) {
              NX::Nexus::ReportException(context->toJSContext(), exp);
            }
            JSValueUnprotect(context->toJSContext(), fun);
          }, scheduler);
           JSValueRef ret = JSValueMakeUndefined(ctx); // NX::Classes::Task::wrapTask(ctx, taskPtr);
           scheduler->scheduleAbstractTask(taskPtr);
           return ret;
        } else {
          NX::Classes::Task * taskObj = NX::Classes::Task::FromObject(fun);
          if (taskObj) {
            scheduler->scheduleAbstractTask(taskObj->task());
          } else {
            *exception = NX::Object(ctx, NX::Exception("argument must be a function or Task instance"));
          }
        }
      } else {
        *exception = NX::Object(ctx, NX::Exception("argument must be a function or Task instance"));
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};

