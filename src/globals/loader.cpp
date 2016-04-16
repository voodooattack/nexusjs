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
#include "globals/loader.h"
#include "globals/promise.h"

#include <boost/asio.hpp>

const JSClassDefinition NX::Globals::Loader::Class {
  0, kJSClassAttributeNone, "Loader", nullptr, nullptr, NX::Globals::Loader::Methods
};


JSValueRef NX::Globals::Loader::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Module * module = reinterpret_cast<NX::Module*>(JSObjectGetPrivate(JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
  if (JSObjectRef Loader = module->getGlobal("Loader"))
    return Loader;
  return module->setGlobal("Loader", JSObjectMake(module->context(), module->defineOrGetClass(NX::Globals::Loader::Class), nullptr));
}

const JSStaticFunction NX::Globals::Loader::Methods[] {
  { "importSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = Module::FromContext(ctx);

      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "import", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = Module::FromContext(ctx);

      JSObjectRef loadFunction = JSObjectMakeFunctionWithCallback(ctx, nullptr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
           size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef
      {
        /* TODO: FINISH THIS */
      });;
      return Promise::createPromise(ctx, loadFunction, exception);
    }, 0
  },
  { "resolve", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = Module::FromContext(ctx);
      /* TODO: IMPLEMENT THIS */
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
