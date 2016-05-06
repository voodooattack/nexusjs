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

#include "classes/emitter.h"

#include <boost/thread/pthread/mutex.hpp>

NX::Global::Global()
{

}

NX::Global::~Global()
{

}

constexpr JSClassDefinition NX::Global::InitGlobalClass()
{
  JSClassDefinition globalDef = kJSClassDefinitionEmpty;
  globalDef.className = "Global";
  globalDef.staticValues = NX::Global::GlobalProperties;
  globalDef.staticFunctions = NX::Global::GlobalFunctions;
  return globalDef;
}

boost::mutex timeoutsMutex;
boost::unordered_map<int, NX::AbstractTask *> globalTimeouts;

JSStaticFunction NX::Global::GlobalFunctions[] {
  { "setTimeout",
    [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
       const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef
    {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Nexus * nx = context->nexus();
      try {
        if (argumentCount < 2) {
          throw std::runtime_error("invalid arguments passed to setTimeout");
        }
        NX::Value timeout(ctx, arguments[1]);
        std::vector<JSValueRef> saved { arguments[0], arguments[1] };
        std::vector<JSValueRef> args;
        JSValueProtect(context->toJSContext(), arguments[0]);
        for(std::size_t i = 2; i < argumentCount; i++) {
          JSValueProtect(context->toJSContext(), arguments[i]);
          args.push_back(arguments[i]);
        }
        NX::AbstractTask * task = nx->scheduler()->scheduleTask(boost::posix_time::milliseconds(timeout.toNumber()), [=]() {
          JSValueRef exp = nullptr;
          JSObjectCallAsFunction(context->toJSContext(), JSValueToObject(context->toJSContext(), saved[0], &exp),
                                 nullptr, args.size(), &args[0], &exp);
          if (exp) {
            NX::Nexus::ReportException(context->toJSContext(), exp);
          }
          for(auto i : args) {
            JSValueUnprotect(context->toJSContext(), i);
          }
          JSValueUnprotect(context->toJSContext(), saved[0]);
        });
        {
          boost::mutex::scoped_lock lock(timeoutsMutex);
          int id = 1;
          while(globalTimeouts.find(id) != globalTimeouts.end()) id++;
          globalTimeouts[id] = task;
          return JSValueMakeNumber(ctx, id);
        }
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
        if (argumentCount != 1) {
          throw std::runtime_error("invalid arguments passed to clearTimeout");
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
//   { "__valueProtect",
//     [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
//        const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//          for(int i = 0; i < argumentCount; i++)
//           JSValueProtect(JSContextGetGlobalContext(ctx), arguments[i]);
//          return JSValueMakeUndefined(ctx);
//        }, 0
//   },
//   { "__valueUnprotect",
//     [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
//        const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//          NX::Context * context = Context::FromJsContext(ctx);
//          for(int i = 0; i < argumentCount; i++)
//            JSValueUnprotect(JSContextGetGlobalContext(ctx), arguments[i]);
//          return JSValueMakeUndefined(ctx);
//        }, 0
//   },
  { nullptr, nullptr, 0 }
};

JSStaticValue NX::Global::GlobalProperties[] {
  { "Nexus", &NX::Global::NexusGet, nullptr, kJSPropertyAttributeNone },
  NX::Globals::Console::GetStaticProperty(),
  NX::Globals::Promise::GetStaticProperty(),
  NX::Globals::Loader::GetStaticProperty(),
  { nullptr, nullptr, nullptr, 0 }
};

JSValueRef NX::Global::NexusGet (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (JSObjectRef nexus = context->getGlobal("Nexus")) {
    return nexus;
  }
  return context->setGlobal("Nexus", JSObjectMake(context->toJSContext(),
                                                  context->nexus()->defineOrGetClass(NX::Global::NexusClass), nullptr));
}

JSClassDefinition NX::Global::GlobalClass = NX::Global::InitGlobalClass();

JSStaticFunction NX::Global::NexusFunctions[] {
  { nullptr, nullptr, 0 }
};

JSStaticValue NX::Global::NexusProperties[] {
  { "Globals", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    JSObjectRef globals = JSObjectMake(ctx, context->nexus()->genericClass(), nullptr);
    boost::unordered_map<std::string, JSValueRef> props {
      { "entryPoint", NX::Value(ctx, context->nexus()->scriptPath()).value() }
    };
    for(std::size_t i = 0; i < props.size(); i++) {
      auto prop = props.begin();
      std::advance(prop, i);;
      NX::ScopedString name(prop->first);
      JSObjectSetProperty(ctx, globals, name, prop->second, kJSPropertyAttributeNone, nullptr);
    }
    return globals;
  }, nullptr, kJSPropertyAttributeNone },
  { "EventEmitter", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    if (JSObjectRef Emitter = context->getGlobal("EventEmitter"))
      return Emitter;
    return context->setGlobal("EventEmitter", NX::Classes::Emitter::getConstructor(context));
  }, nullptr, kJSPropertyAttributeNone },
  NX::Globals::Scheduler::GetStaticProperty(),
  NX::Globals::IO::GetStaticProperty(),
  NX::Globals::FileSystem::GetStaticProperty(),
  NX::Globals::Context::GetStaticProperty(),
  NX::Globals::Module::GetStaticProperty(),
};

JSClassDefinition NX::Global::NexusClass {
  0, kJSClassAttributeNone, "Nexus", nullptr, NX::Global::NexusProperties, NX::Global::NexusFunctions
};
