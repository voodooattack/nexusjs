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
#include "globals/net.h"
#include "classes/net/tcp/acceptor.h"
#include "classes/net/http/server.h"
#include "classes/net/http2/server.h"

JSValueRef NX::Globals::Net::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (JSObjectRef Net = context->getGlobal("Nexus.Net")) {
    return Net;
  }
  return context->setGlobal("Nexus.Net", JSObjectMake(context->toJSContext(),
                                                     context->nexus()->defineOrGetClass(NX::Globals::Net::Class),
                                                     nullptr));
}

const JSClassDefinition NX::Globals::Net::Class {
  0, kJSClassAttributeNone, "Net", nullptr, NX::Globals::Net::Properties, NX::Globals::Net::Methods
};

const JSStaticValue NX::Globals::Net::Properties[] {
  { "TCP", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    if (JSObjectRef TCP = context->getGlobal("Nexus.Net.TCP"))
      return TCP;
    JSObjectRef tcpObject = JSObjectMake(ctx, context->nexus()->defineOrGetClass(NX::Globals::Net::TCPClass), nullptr);
    context->setGlobal("Nexus.Net.TCP", tcpObject);
    return tcpObject;
  },
  nullptr, kJSPropertyAttributeNone },
  { "HTTP", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    if (JSObjectRef HTTP = context->getGlobal("Nexus.Net.HTTP"))
      return HTTP;
    JSObjectRef tcpObject = JSObjectMake(ctx, context->nexus()->defineOrGetClass(NX::Globals::Net::HTTPClass), nullptr);
    context->setGlobal("Nexus.Net.HTTP", tcpObject);
    return tcpObject;
  },
  nullptr, kJSPropertyAttributeNone },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Net::Methods[] {
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Globals::Net::TCPClass {
  0, kJSClassAttributeNone, "TCP", nullptr, NX::Globals::Net::TCPProperties, NX::Globals::Net::TCPMethods
};

const JSStaticValue NX::Globals::Net::TCPProperties[] {
  { "Acceptor", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    if (JSObjectRef Acceptor = context->getGlobal("Nexus.Net.TCP.Acceptor"))
      return Acceptor;
    JSObjectRef ctor = NX::Classes::Net::TCP::Acceptor::getConstructor(context);
    context->setGlobal("Nexus.Net.TCP.Acceptor", ctor);
    return ctor;
  },
  nullptr, kJSPropertyAttributeNone },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Net::TCPMethods[] {
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Globals::Net::HTTPClass {
  0, kJSClassAttributeNone, "HTTP", nullptr, NX::Globals::Net::HTTPProperties, NX::Globals::Net::HTTPMethods
};

const JSStaticValue NX::Globals::Net::HTTPProperties[] {
  { "Server", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    if (JSObjectRef Server = context->getGlobal("Nexus.Net.HTTP.Server"))
      return Server;
    JSObjectRef ctor = NX::Classes::Net::HTTP::Server::getConstructor(context);
    context->setGlobal("Nexus.Net.HTTP.Server", ctor);
    return ctor;
  },
  nullptr, kJSPropertyAttributeNone },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Net::HTTPMethods[] {
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Globals::Net::HTTP2Class {
  0, kJSClassAttributeNone, "TCP", nullptr, NX::Globals::Net::HTTP2Properties, NX::Globals::Net::HTTP2Methods
};

const JSStaticValue NX::Globals::Net::HTTP2Properties[] {
  { "Server", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Context * context = Context::FromJsContext(ctx);
    if (JSObjectRef Server = context->getGlobal("Nexus.Net.HTTP2.Server"))
      return Server;
    JSObjectRef ctor = NX::Classes::Net::HTTP2::Server::getConstructor(context);
    context->setGlobal("Nexus.Net.HTTP2.Server", ctor);
    return ctor;
  },
  nullptr, kJSPropertyAttributeNone },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Net::HTTP2Methods[] {
  { nullptr, nullptr, 0 }
};


