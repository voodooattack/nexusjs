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
  NX::Context * context = Context::FromJsContext(ctx);
  if (JSObjectRef Scheduler = context->getGlobal("Nexus.Scheduler"))
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
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Scheduler::Methods[] {
  { "schedule", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Scheduler * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(thisObject));
      if (argumentCount != 1) {
        *exception = NX::Object(ctx, std::runtime_error("Scheduler.schedule called without an argument"));
        return JSValueMakeUndefined(ctx);
      }
      if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject) {
        *exception = NX::Object(ctx, std::runtime_error("invalid argument passed to Scheduler.schedule"));
        return JSValueMakeUndefined(ctx);
      }
      JSObjectRef fun = JSValueToObject(context->toJSContext(), arguments[0], exception);
      if (fun) {
        JSValueProtect(context->toJSContext(), fun);
        NX::AbstractTask * taskPtr = scheduler->scheduleTask([=]() {
          JSValueRef exp = nullptr;
          JSObjectCallAsFunction(context->toJSContext(), fun, nullptr, 0, nullptr, &exp);
          if (exp) {
            NX::Nexus::ReportException(context->toJSContext(), exp);
          }
          JSValueUnprotect(context->toJSContext(), fun);
        });
        if (!taskPtr) {
          *exception =  NX::Object(ctx, std::runtime_error("task could not be scheduled"));
        }
      } else {
        *exception =  NX::Object(ctx, std::runtime_error("argument must be a function"));
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};

