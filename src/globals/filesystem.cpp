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
#include "context.h"
#include "object.h"
#include "globals/filesystem.h"
#include "classes/file.h"

JSValueRef NX::Globals::FileSystem::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (JSObjectRef FileSystem = context->getGlobal("Nexus.FileSystem")) {
    return FileSystem;
  }
  return context->setGlobal("Nexus.FileSystem", JSObjectMake(context->toJSContext(), context->defineOrGetClass(NX::Globals::FileSystem::Class), nullptr));
}

const JSClassDefinition NX::Globals::FileSystem::Class {
  0, kJSClassAttributeNone, "FileSystem", nullptr, NX::Globals::FileSystem::Properties, NX::Globals::FileSystem::Methods
};

const JSStaticValue NX::Globals::FileSystem::Properties[] {
  { "File", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("File"))
        return File;
      JSObjectRef constructor = NX::Classes::File::getConstructor(context);
      context->setGlobal("File", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "OpenMode", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef OpenMode = context->getGlobal("Nexus.FileSystem.OpenMode"))
        return OpenMode;
      NX::Object modes(ctx);
      modes.set("Read", JSValueMakeNumber(ctx, std::fstream::in));
      modes.set("Write", JSValueMakeNumber(ctx, std::fstream::out));
      modes.set("Binary", JSValueMakeNumber(ctx, std::fstream::binary));
      modes.set("End", JSValueMakeNumber(ctx, std::fstream::ate));
      modes.set("Append", JSValueMakeNumber(ctx, std::fstream::app));
      modes.set("Truncate", JSValueMakeNumber(ctx, std::fstream::trunc));
      return context->setGlobal("Nexus.FileSystem.OpenMode", modes.value());
    }, nullptr, kJSPropertyAttributeNone
  },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::FileSystem::Methods[] {
  { "exists", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      /* TODO: IMPLEMENT THIS */
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "resolve", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      /* TODO: IMPLEMENT THIS */
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
