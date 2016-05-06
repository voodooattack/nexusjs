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

#include "classes/io/device.h"
#include "classes/io/filter.h"
#include "classes/io/stream.h"
#include "classes/io/devices/socket.h"
#include "classes/io/devices/file.h"
#include "classes/io/filters/encoding.h"
#include "classes/io/filters/utf8stringfilter.h"

JSValueRef NX::Globals::IO::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (JSObjectRef IO = context->getGlobal("Nexus.IO")) {
    return IO;
  }
  return context->setGlobal("Nexus.IO", JSObjectMake(context->toJSContext(),
                                                             context->nexus()->defineOrGetClass(NX::Globals::IO::Class),
                                                             nullptr));
}

const JSClassDefinition NX::Globals::IO::Class {
  0, kJSClassAttributeNone, "IO", nullptr, NX::Globals::IO::Properties, NX::Globals::IO::Methods
};

const JSStaticValue NX::Globals::IO::Properties[] {
  { "Filter", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.Filter"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Filter::getConstructor(context);
      context->setGlobal("Nexus.IO.Filter", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "FilePullDevice", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.FilePullDevice"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Devices::FilePullDevice::getConstructor(context);
      context->setGlobal("Nexus.IO.FilePullDevice", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "FilePushDevice", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.FilePushDevice"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Devices::FilePushDevice::getConstructor(context);
      context->setGlobal("Nexus.IO.FilePushDevice", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "FileSinkDevice", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.FileSinkDevice"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Devices::FileSinkDevice::getConstructor(context);
      context->setGlobal("Nexus.IO.FileSinkDevice", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "SocketDevice", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.Socket"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Devices::Socket::getConstructor(context);
      context->setGlobal("Nexus.IO.Socket", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "UDPSocketDevice", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.UDPSocket"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Devices::UDPSocket::getConstructor(context);
      context->setGlobal("Nexus.IO.UDPSocket", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "ReadableStream", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.ReadableStream"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::ReadableStream::getConstructor(context);
      context->setGlobal("Nexus.IO.ReadableStream", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "WritableStream", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.WritableStream"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::WritableStream::getConstructor(context);
      context->setGlobal("Nexus.IO.WritableStream", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "EncodingConversionFilter", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.EncodingConversionFilter"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Filters::EncodingConversionFilter::getConstructor(context);
      context->setGlobal("Nexus.IO.EncodingConversionFilter", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { "UTF8StringFilter", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (JSObjectRef File = context->getGlobal("Nexus.IO.UTF8StringFilter"))
        return File;
      JSObjectRef constructor = NX::Classes::IO::Filters::UTF8StringFilter::getConstructor(context);
      context->setGlobal("Nexus.IO.UTF8StringFilter", constructor);
      return constructor;
    },
    nullptr, kJSPropertyAttributeNone },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::IO::Methods[] {
  { nullptr, nullptr, 0 }
};
