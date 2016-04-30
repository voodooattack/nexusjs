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

#include "classes/io/devices/socket.h"

JSObjectRef NX::Classes::IO::Devices::Socket::Constructor (JSContextRef ctx, JSObjectRef constructor,
                                                           size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{

}

JSClassRef NX::Classes::IO::Devices::Socket::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::Devices::Socket::Class;
  def.parentClass = NX::Classes::IO::BidirectionalPushDevice::createClass (context);
  return context->nexus()->defineOrGetClass (def);
}

JSObjectRef NX::Classes::IO::Devices::Socket::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Devices::Socket::Constructor);
}

const JSClassDefinition NX::Classes::IO::Devices::Socket::Class {
  0, kJSClassAttributeNone, "Socket", nullptr, NX::Classes::IO::Devices::Socket::Properties,
  NX::Classes::IO::Devices::Socket::Methods, nullptr, NX::Classes::IO::Devices::Socket::Finalize
};

const JSStaticValue NX::Classes::IO::Devices::Socket::Properties[] {
  { "available", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Classes::IO::Devices::Socket * socket = NX::Classes::IO::Devices::Socket::FromObject(object);
    if (!socket) {
      NX::Value message(ctx, "available not implemented on Socket instance");
      JSValueRef args[] { message.value(), nullptr };
      *exception = JSObjectMakeError(ctx, 1, args, nullptr);
      return JSValueMakeUndefined(ctx);
    }
    return JSValueMakeNumber(ctx, socket->available());
  }, nullptr, 0 },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::Devices::Socket::Methods[] {
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Classes::IO::Devices::TCPSocket::Class {
  0, kJSClassAttributeNone, "TCPSocket", nullptr, NX::Classes::IO::Devices::TCPSocket::Properties,
  NX::Classes::IO::Devices::TCPSocket::Methods, nullptr, NX::Classes::IO::Devices::TCPSocket::Finalize
};

const JSStaticValue NX::Classes::IO::Devices::TCPSocket::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::Devices::TCPSocket::Methods[] {
  { nullptr, nullptr, 0 }
};

