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

#include "context.h"
#include "globals/io.h"
#include "classes/io/file.h"

JSValueRef NX::Globals::IO::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (JSObjectRef IO = context->getGlobal("Nexus.IO")) {
    return IO;
  }
  return context->setGlobal("Nexus.IO", JSObjectMake(context->toJSContext(),
                                                             context->defineOrGetClass(NX::Globals::IO::Class),
                                                             nullptr));
}

const JSClassDefinition NX::Globals::IO::Class {
  0, kJSClassAttributeNone, "IO", nullptr, NX::Globals::IO::Properties, NX::Globals::IO::Methods
};

const JSStaticValue NX::Globals::IO::Properties[] {
  { "FileSourceDevice", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("IO.FileSourceDevice"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::FileSourceDevice::getConstructor(context);
      context->setGlobal("IO.FileSourceDevice", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::IO::Methods[] {
  { nullptr, nullptr, 0 }
};
