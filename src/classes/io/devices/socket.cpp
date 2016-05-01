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

#include "globals/promise.h"
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
  { "close", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::Devices::Socket * socket = NX::Classes::IO::Devices::Socket::FromObject(thisObject);
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        socket->close();
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "cancel", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::Devices::Socket * socket = NX::Classes::IO::Devices::Socket::FromObject(thisObject);
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        socket->cancel();
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "connect", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::Devices::Socket * socket = NX::Classes::IO::Devices::Socket::FromObject(thisObject);
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount < 1)
          throw std::runtime_error("invalid arguments");
        std::string addr = NX::Value(ctx, arguments[0]).toString();
        return socket->connect(ctx, thisObject, addr, exception);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
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

JSObjectRef NX::Classes::IO::Devices::UDPSocket::connect (JSContextRef ctx, JSObjectRef thisObject, const std::string & address, JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSValueProtect(context->toJSContext(), thisObject);
  return Globals::Promise::createPromise(ctx, [=](NX::Context *, NX::ResolveRejectHandler resolve, NX::ResolveRejectHandler reject) {
    typedef boost::asio::ip::udp::resolver resolver;
    std::shared_ptr<resolver> res(new resolver(*myScheduler->service()));
    res->async_resolve(boost::asio::ip::udp::resolver::query(address, ""), [=](const auto & error, const auto & it)
    {
      if (error) {
        JSValueUnprotect(context->toJSContext(), thisObject);
        return reject(NX::Object(context->toJSContext(), error));
      }
      else {
        boost::asio::async_connect(*mySocket, it, boost::asio::ip::udp::resolver::iterator(), [=](const auto & error, const auto & next) {
          JSValueRef args[] {
            NX::Value(context->toJSContext(), next->host_name()).value(),
            NX::Value(context->toJSContext(), next->service_name()).value()
          };
          this->emitFast(context->toJSContext(), thisObject, "attempt", 2, args, nullptr);
          return next;
        }, [=](const auto & error, const auto & it) {
          JSValueUnprotect(context->toJSContext(), thisObject);
          if (error) {
            return reject(NX::Object(context->toJSContext(), error));
          }
          else {
            JSValueRef args[] {
              NX::Value(context->toJSContext(), it->host_name()).value(),
              NX::Value(context->toJSContext(), it->service_name()).value()
            };
            myEndpoint = *it;
            this->emitFast(context->toJSContext(), thisObject, "connected", 2, args, nullptr);
            return resolve(thisObject);
          }
        });
      }
    });
  });
}

JSObjectRef NX::Classes::IO::Devices::UDPSocket::resume (JSContextRef ctx, JSObjectRef thisObject)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSValueProtect(context->toJSContext(), thisObject);
  return Globals::Promise::createPromise(ctx, [=](NX::Context *, NX::ResolveRejectHandler resolve, NX::ResolveRejectHandler reject) {
    const std::size_t bufSize = 1024;
    auto recvHandler = [=](auto next, char * buffer, std::size_t len, const boost::system::error_code& ec, std::size_t bytes_transferred) -> void {
      if (ec) {
        if (buffer) std::free(buffer);
        JSValueUnprotect(context->toJSContext(), thisObject);
        reject(NX::Object(context->toJSContext(), ec));
      }
      else
      {
        if (buffer) {
          JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(), buffer, bytes_transferred, [](auto ptr, auto ctx) {
            std::free(ptr);
          }, nullptr, nullptr);
          JSValueRef args[] { arrayBuffer, JSValueMakeUndefined(context->toJSContext()) };
          JSValueRef exp = nullptr;
          this->emitFast(context->toJSContext(), thisObject, "data", 2, args, &exp);
          if (exp) {
            JSValueUnprotect(context->toJSContext(), thisObject);
            return reject(exp);
          }
        }
        char * buf = (char *)std::malloc(bufSize);
        mySocket->async_receive(boost::asio::buffer(buf, bufSize),
          boost::bind<void>(next, next, buf, bufSize, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
      }
    };
    recvHandler(recvHandler, nullptr, 0, boost::system::error_code(), 0);
  });
}
