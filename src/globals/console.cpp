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
#include "globals/console.h"

#include <iostream>

JSValueRef NX::Globals::Console::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Nexus * nx = reinterpret_cast<NX::Nexus*>(JSObjectGetPrivate(object));
  return JSObjectMake(nx->context(), nx->defineOrGetClass(NX::Globals::Console::Class), nullptr);
}

const JSStaticFunction NX::Globals::Console::Methods[] {
  { "log", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      std::string output;
      for(int i = 0; i < argumentCount; i++)
      {
        output += NX::Value(ctx, arguments[i]).toString();
        if (i < argumentCount - 1)
          output += " ";
      }
      output += "\n";
      std::cout << output;
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "error", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      std::string output;
      for(int i = 0; i < argumentCount; i++)
      {
        output += NX::Value(ctx, arguments[i]).toString();
        if (i < argumentCount - 1)
          output += " ";
      }
      output += "\n";
      std::cerr << output;
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Globals::Console::Class {
  0, kJSClassAttributeNone, "Console", nullptr, nullptr, NX::Globals::Console::Methods
};
