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

#include "nexus.h"
#include "value.h"
#include "task.h"
#include "scoped_string.h"

#include "globals/global.h"
#include "globals/console.h"
#include "globals/scheduler.h"
#include "globals/promise.h"
#include "globals/module.h"
#include "globals/loader.h"
#include "globals/filesystem.h"
#include "globals/context.h"
#include "globals/io.h"
#include "globals/net.h"

#include "classes/emitter.h"

#include <boost/thread/pthread/mutex.hpp>

constexpr JSClassDefinition NX::Global::InitGlobalClass()
{
  JSClassDefinition globalDef = kJSClassDefinitionEmpty;
  globalDef.className = "Global";
  globalDef.staticValues = NX::Global::GlobalProperties;
  globalDef.staticFunctions = NX::Global::GlobalFunctions;
  return globalDef;
}

static boost::mutex timeoutsMutex;
static boost::unordered_map<int, NX::AbstractTask *> globalTimeouts;

const JSStaticFunction NX::Global::GlobalFunctions[] {
  { "setTimeout",
    [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
       const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef
    {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Nexus * nx = context->nexus();
      JSC::JSLockHolder lock(toJS(ctx));
      try {
        if (argumentCount < 2) {
          NX::Object exp(ctx, NX::Exception("invalid arguments passed to setTimeout"));
          *exception = exp.value();
          return JSValueMakeUndefined(ctx);
        }
        NX::Value timeout(ctx, arguments[1]);
        NX::ProtectedArguments saved (context->toJSContext(), 2, arguments);
        NX::ProtectedArguments args(context->toJSContext(), argumentCount - 2, arguments + 2);
        JSValueProtect(context->toJSContext(), arguments[0]);
        NX::AbstractTask * task = nx->scheduler()->scheduleTask(boost::posix_time::milliseconds((unsigned)timeout.toNumber()), [=]() {
          JSValueRef exp = nullptr;
          JSObjectCallAsFunction(context->toJSContext(), JSValueToObject(context->toJSContext(), saved[0], &exp),
                                 nullptr, args.size(), args, &exp);
          if (exp) {
            NX::Nexus::ReportException(context->toJSContext(), exp);
          }
        });
        int id = 1;
        while(globalTimeouts.find(id) != globalTimeouts.end()) id++;
        globalTimeouts[id] = task;
        return JSValueMakeNumber(ctx, id);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "clearTimeout",
    [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
       const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        JSC::JSLockHolder lock(toJS(ctx));
        if (argumentCount != 1) {
          return JSValueMakeUndefined(ctx);
        }
        NX::Value timeoutId(ctx, arguments[0]);
        int taskId = (int)timeoutId.toNumber();
        {
          boost::mutex::scoped_lock lock(timeoutsMutex);
          if (NX::AbstractTask * task = globalTimeouts[taskId]) {
            task->abort();
          } else {
            /* TODO error */
          }
          globalTimeouts.erase(taskId);
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};

const JSStaticValue NX::Global::GlobalProperties[] {
  { "global", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName,
                 JSValueRef * exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    return context->globalThisValue();
  }, nullptr, kJSPropertyAttributeNone },
  { "Nexus", &NX::Global::NexusGet, nullptr, kJSPropertyAttributeNone },
  NX::Globals::Console::GetStaticProperty(),
//  NX::Globals::Promise::GetStaticProperty(),
//  NX::Globals::Loader::GetStaticProperty(),
  { nullptr, nullptr, nullptr, 0 }
};

JSValueRef NX::Global::NexusGet (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (auto value = context->getGlobal("Nexus")) {
    return value;
  }
  return context->setGlobal("Nexus", JSObjectMake(context->toJSContext(),
                                                  context->nexus()->defineOrGetClass(NX::Global::NexusClass), exception));
}

const JSClassDefinition NX::Global::GlobalClass = NX::Global::InitGlobalClass();

const JSStaticFunction NX::Global::NexusFunctions[] {
  { nullptr, nullptr, 0 }
};

const JSStaticValue NX::Global::NexusProperties[] {
  { "version", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    return NX::Value(ctx, NEXUS_VERSION).value();
  }, nullptr, kJSPropertyAttributeNone },
  { "script", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    return NX::Value(ctx, context->nexus()->scriptPath()).value();
  }, nullptr, kJSPropertyAttributeNone },
  { "EventEmitter", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    if (auto val = context->getGlobal("EventEmitter"))
      return val;
    return context->setGlobal("EventEmitter", NX::Classes::Emitter::getConstructor(context));
  }, nullptr, kJSPropertyAttributeNone },
  NX::Globals::Scheduler::GetStaticProperty(),
  NX::Globals::IO::GetStaticProperty(),
  NX::Globals::Net::GetStaticProperty(),
  NX::Globals::FileSystem::GetStaticProperty(),
  NX::Globals::Context::GetStaticProperty(),
  NX::Globals::Module::GetStaticProperty(),
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Global::NexusClass {
  0, kJSClassAttributeNone, "Nexus", nullptr, NX::Global::NexusProperties, NX::Global::NexusFunctions
};
