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
        return *exception = NX::Exception("bind() not implemented on Acceptor instance").toError(ctx);
      }
      try {
        if (argumentCount < 2) {
          return *exception = NX::Exception("invalid arguments passed to Acceptor.bind").toError(ctx);
        }
        std::string addr = NX::Value(ctx, arguments[0]).toString();
        auto port = static_cast<unsigned short>(NX::Value(ctx, arguments[1]).toNumber());
        auto reuse = argumentCount > 2 ? NX::Value(ctx, arguments[2]).toBoolean() : false;
        return acceptor->bind(ctx, thisObject, addr, port, reuse, exception);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "listen", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef
    {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      NX::Classes::Net::TCP::Acceptor * acceptor = NX::Classes::Net::TCP::Acceptor::FromObject(thisObject);
      if (!acceptor) {
        return *exception = NX::Exception("listen() not implemented on Acceptor instance").toError(ctx);
      }
      try {
        unsigned maxConnections = boost::asio::socket_base::max_connections;
        if (argumentCount >= 1) {
          NX::Value arg(ctx, arguments[0]);
          maxConnections = static_cast<unsigned>(arg.toNumber());
        }
        JSValueRef ret = acceptor->listen(ctx, NX::Object(context->toJSContext(), thisObject), maxConnections, exception);
        return ret;
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSValueRef NX::Classes::Net::TCP::Acceptor::bind(JSContextRef ctx, JSObjectRef thisObject,
                                                 const std::string & addr, unsigned short port, bool reuse, JSValueRef * exception)
{
  typedef boost::asio::ip::tcp::endpoint Endpoint;
  Endpoint endpoint(boost::asio::ip::address::from_string(addr), port);
  try {
    if (!myAcceptor->is_open())
      myAcceptor->open(endpoint.protocol());
    myAcceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(reuse));
    myAcceptor->bind(endpoint);
  } catch(const std::exception & e) {
    return JSWrapException(ctx, e, exception);
  }
  return thisObject;
}

JSValueRef NX::Classes::Net::TCP::Acceptor::listen(JSContextRef ctx, const NX::Object & thisObject, int maxConnections, JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  if (!myThisObject)
    myThisObject = thisObject;
  try {
    myAcceptor->listen(maxConnections);
    beginAccept(context, thisObject);
  } catch(const std::exception & e) {
    JSWrapException(ctx, e, exception);
  }
  return thisObject;
}

void NX::Classes::Net::TCP::Acceptor::beginAccept(NX::Context * context, const NX::Object & thisObject)
{
  auto socket = std::make_shared<boost::asio::ip::tcp::socket>(*myScheduler->service());
  myAcceptor->async_accept(*socket, std::bind(&Acceptor::handleAccept, this, context,
                                              NX::Object(context->toJSContext(), thisObject), socket, false, std::placeholders::_1));
}

void NX::Classes::Net::TCP::Acceptor::handleAccept(NX::Context* context, const NX::Object & thisObject,
                                                          const std::shared_ptr< boost::asio::ip::tcp::socket > & socket,
                                                          bool continuation, const boost::system::error_code& error)
{
  if (!myThisObject)
    myThisObject = NX::Object(context->toJSContext(), thisObject);
  if (!continuation) {
    beginAccept(context, thisObject);
  }
  if (error) {
    JSValueRef args[] { NX::Object(context->toJSContext(), error )};
    emitFastAndSchedule(context->toJSContext(), thisObject, "error", 1, args, nullptr);
  }
  NX::Object thisObj(myThisObject);
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
      remoteEndpoint.value(),
      thisObj
    };
    JSValueRef exception = nullptr;
    emitFastAndSchedule(context->toJSContext(), thisObj, "connection", 3, arguments, &exception);
    if (exception) {
      NX::Nexus::ReportException(context->toJSContext(), exception);
    }
  }
}
