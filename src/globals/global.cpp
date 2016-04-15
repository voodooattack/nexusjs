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
#include "globals/global.h"
#include "globals/console.h"
#include "globals/scheduler.h"
#include "globals/promise.h"
#include "globals/module.h"
#include "globals/loader.h"

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
    [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = reinterpret_cast<NX::Module*>(JSObjectGetPrivate(JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
      NX::Nexus * nx = module->nexus();
      if (argumentCount < 2) {
        // TODO ERROR
      }
      NX::Value timeout(ctx, arguments[1]);
      std::vector<JSValueRef> saved { arguments[0], arguments[1] };
      std::vector<JSValueRef> args;
      JSValueProtect(module->context(), arguments[0]);
      for(int i = 2; i < argumentCount; i++) {
        JSValueProtect(module->context(), arguments[i]);
        args.push_back(arguments[i]);
      }
      NX::AbstractTask * task = nx->scheduler()->scheduleTask(boost::posix_time::milliseconds(timeout.toNumber()), [=]() {
        JSValueRef exp = nullptr;
        JSValueRef ret = JSObjectCallAsFunction(module->context(), JSValueToObject(module->context(), saved[0], &exp),
                                                nullptr, args.size(), &args[0], &exp);
        if (exp) {
          NX::Nexus::ReportException(module->context(), exp);
        }
        for(auto i : args) {
          JSValueUnprotect(module->context(), i);
        }
        JSValueUnprotect(module->context(), saved[0]);
      });
      {
        boost::mutex::scoped_lock lock(timeoutsMutex);
        int id = 1;
        while(globalTimeouts.find(id) != globalTimeouts.end()) id++;
        globalTimeouts[id] = task;
        return JSValueMakeNumber(ctx, id);
      }
    }, 0
  },
  { "clearTimeout",
    [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Nexus * nx = reinterpret_cast<NX::Nexus*>(JSObjectGetPrivate(thisObject));
      if (argumentCount != 1) {
        // TODO ERROR
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
        return JSValueMakeUndefined(ctx);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSStaticValue NX::Global::GlobalProperties[] {
  NX::Globals::Console::GetStaticProperty(),
  NX::Globals::Scheduler::GetStaticProperty(),
  NX::Globals::Promise::GetStaticProperty(),
  NX::Globals::Module::GetStaticProperty(),
  NX::Globals::Loader::GetStaticProperty(),
  { nullptr, nullptr, nullptr, 0 }
};

JSClassDefinition NX::Global::GlobalClass = NX::Global::InitGlobalClass();
