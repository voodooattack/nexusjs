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

#include "globals/system.h"
#include "nexus.h"

const JSClassDefinition NX::Globals::System::Class {
  0, kJSClassAttributeNone, "System", nullptr, nullptr, NX::Globals::System::Methods
};


JSValueRef NX::Globals::System::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Module * module = reinterpret_cast<NX::Module*>(JSObjectGetPrivate(JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
  if (module->globals().find("System") != module->globals().end())
    return module->globals()["System"];
  return module->globals()["System"] = JSObjectMake(module->context(), module->nexus()->defineOrGetClass(NX::Globals::System::Class), nullptr);
}

const JSStaticFunction NX::Globals::System::Methods[] {
  { "import", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = reinterpret_cast<NX::Module*>(JSObjectGetPrivate(JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
      /* TODO: IMPLEMENT THIS */
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "resolve", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = reinterpret_cast<NX::Module*>(JSObjectGetPrivate(JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
      /* TODO: IMPLEMENT THIS */
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
