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

#include "classes/net/tcp/acceptor.h"
#include "classes/io/devices/socket.h"

const JSClassDefinition NX::Classes::Net::TCP::Acceptor::Class {
  0, kJSClassAttributeNone, "Acceptor", nullptr, NX::Classes::Net::TCP::Acceptor::Properties,
  NX::Classes::Net::TCP::Acceptor::Methods, nullptr, NX::Classes::Net::TCP::Acceptor::Finalize
};

const JSStaticValue NX::Classes::Net::TCP::Acceptor::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Net::TCP::Acceptor::Methods[] {
  { "bind", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::Net::TCP::Acceptor * acceptor = NX::Classes::Net::TCP::Acceptor::FromObject(thisObject);
      if (!acceptor) {
        NX::Value message(ctx, "bind() not implemented on Acceptor instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      try {
        if (argumentCount < 1)
          throw std::runtime_error("invalid arguments passed to Acceptor.bind");
        std::string addr = NX::Value(ctx, arguments[0]).toString();
        unsigned short port = NX::Value(ctx, arguments[1]).toNumber();
        JSValueRef ret = acceptor->bind(ctx, thisObject, addr, port, exception);
        return ret;
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "listen", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::Net::TCP::Acceptor * acceptor = NX::Classes::Net::TCP::Acceptor::FromObject(thisObject);
      if (!acceptor) {
        NX::Value message(ctx, "listen() not implemented on Acceptor instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      try {
        int maxConnections = boost::asio::socket_base::max_connections;
        if (argumentCount >= 1) {
          NX::Value arg(ctx, arguments[0]);
          maxConnections = arg.toNumber();
        }
        JSValueRef ret = acceptor->listen(ctx, thisObject, maxConnections, exception);
        return ret;
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSValueRef NX::Classes::Net::TCP::Acceptor::bind(JSContextRef ctx, JSObjectRef thisObject,
                                                 const std::string & addr, unsigned short port, JSValueRef * exception)
{
  typedef boost::asio::ip::tcp::endpoint Endpoint;
  Endpoint endpoint(boost::asio::ip::address::from_string(addr), port);
  try {
    if (!myAcceptor->is_open())
      myAcceptor->open(endpoint.protocol());
    myAcceptor->bind(endpoint);
  } catch(const std::exception & e) {
    return JSWrapException(ctx, e, exception);
  }
  return thisObject;
}

JSValueRef NX::Classes::Net::TCP::Acceptor::listen(JSContextRef ctx, JSObjectRef thisObject, int maxConnections, JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSValueProtect(context->toJSContext(), thisObject);
  myScheduler->hold();
  try {
    myAcceptor->listen(maxConnections);
    beginAccept(context, thisObject);
  } catch(const std::exception & e) {
    JSValueUnprotect(context->toJSContext(), thisObject);
    return JSWrapException(ctx, e, exception);
  }
  return thisObject;
}

void NX::Classes::Net::TCP::Acceptor::beginAccept(NX::Context * context, JSObjectRef thisObject)
{
  std::shared_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(*myScheduler->service()));
  JSValueProtect(context->toJSContext(), thisObject);
  myAcceptor->async_accept(*socket, std::bind(&Acceptor::handleAccept, this, context, thisObject, socket));
}

void NX::Classes::Net::TCP::Acceptor::handleAccept(NX::Context* context, JSObjectRef thisObject, const std::shared_ptr< boost::asio::ip::tcp::socket > & socket)
{
  beginAccept(context, thisObject);
  if (socket->is_open()) {
    NX::Object remoteEndpoint(context->toJSContext());
    try {
      remoteEndpoint.set("address", NX::Value(context->toJSContext(), socket->remote_endpoint().address().to_string()).value());
      remoteEndpoint.set("port", NX::Value(context->toJSContext(), socket->remote_endpoint().port()).value());
    } catch(const std::exception & e) {
      remoteEndpoint.set("address", JSValueMakeUndefined(context->toJSContext()));
      remoteEndpoint.set("port", JSValueMakeUndefined(context->toJSContext()));
    }
    const JSValueRef arguments[] {
      NX::Classes::IO::Devices::TCPSocket::wrapSocket(context, socket),
      remoteEndpoint.value()
    };
    JSValueRef exception = nullptr;
    emitFastAndSchedule(context->toJSContext(), thisObject, "connection", 2, arguments, &exception);
    JSValueUnprotect(context->toJSContext(), thisObject);
    if (exception) {
      NX::Nexus::ReportException(context->toJSContext(), exception);
    }
  }
}
