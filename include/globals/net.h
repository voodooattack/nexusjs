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


#ifndef GLOBALS_NET_H
#define GLOBALS_NET_H

#include <JavaScript.h>

namespace NX {
  class Nexus;
  namespace Globals {
    class Net
    {
      static const JSClassDefinition Class;
      static const JSStaticFunction Methods[];
      static const JSStaticValue Properties[];

      static const JSClassDefinition TCPClass;
      static const JSStaticFunction TCPMethods[];
      static const JSStaticValue TCPProperties[];

      static const JSClassDefinition HTTPClass;
      static const JSStaticFunction HTTPMethods[];
      static const JSStaticValue HTTPProperties[];

      static const JSClassDefinition HTTP2Class;
      static const JSStaticFunction HTTP2Methods[];
      static const JSStaticValue HTTP2Properties[];

      static JSValueRef Get(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception);
    public:
      static constexpr JSStaticValue GetStaticProperty() {
        return JSStaticValue { "Net", &NX::Globals::Net::Get, nullptr, kJSPropertyAttributeNone };
      }
    };
  }
}

#endif // GLOBALS_NET_H
