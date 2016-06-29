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

#ifndef CLASSES_IO_DEVICES_SOCKET_H
#define CLASSES_IO_DEVICES_SOCKET_H

#include "classes/io/device.h"
#include "scheduler.h"
#include "globals/promise.h"

#include <JavaScript.h>
#include <memory>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace NX {
  namespace Classes {
    namespace IO {
      namespace Devices {
        class Socket: public virtual BidirectionalPushDevice
        {
        protected:
          Socket() {}
        public:
          virtual ~Socket() { }

        private:
          static const JSClassDefinition Class;
          static const JSStaticValue Properties[];
          static const JSStaticFunction Methods[];

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                        const JSValueRef arguments[], JSValueRef* exception);

          static void Finalize(JSObjectRef object) { }

        public:
          static JSClassRef createClass(NX::Context * context);
          static JSObjectRef getConstructor(NX::Context * context);

          static NX::Classes::IO::Devices::Socket * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::Socket*>(NX::Classes::Base::FromObject(obj));
          }

          virtual std::size_t available() const = 0;

          virtual void close() = 0;
          virtual void cancel() = 0;

          virtual JSObjectRef connect(JSContextRef ctx, JSObjectRef thisObject, const std::string & address, const std::string & port, JSValueRef * exception) = 0;

        };

        class TCPSocket: public virtual Socket {
        public:
          TCPSocket ( NX::Scheduler * scheduler, const std::shared_ptr<boost::asio::ip::tcp::socket> & socket):
            myScheduler(scheduler), mySocket(socket)
          {
          }
          virtual ~TCPSocket()  {}
        private:
          static const JSClassDefinition Class;
          static const JSStaticValue Properties[];
          static const JSStaticFunction Methods[];

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef* exception);

          static void Finalize(JSObjectRef object) { }

        public:
          static JSClassRef createClass(NX::Context * context);
          static JSObjectRef getConstructor(NX::Context * context);
          
          static JSObjectRef wrapSocket(NX::Context * context, const std::shared_ptr<boost::asio::ip::tcp::socket> & socket);

          static NX::Classes::IO::Devices::TCPSocket * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::TCPSocket*>(NX::Classes::Base::FromObject(obj));
          }


          virtual std::size_t available() const { return mySocket->available(); }
          virtual void cancel() { mySocket->cancel(); }
          virtual void close() { mySocket->close(); }
          virtual JSObjectRef connect (JSContextRef ctx, JSObjectRef thisObject, const std::string & address, const std::string & port, JSValueRef * exception);
          virtual bool deviceReady() const { return mySocket->is_open(); }
          virtual std::size_t maxWriteBufferSize() const { return 65507; }
          virtual std::size_t recommendedWriteBufferSize() const { return maxWriteBufferSize() * 0.5; }
          virtual void deviceWrite ( const char * buffer, std::size_t length ) {
            const std::size_t maxBufferLength = maxWriteBufferSize();
            for(std::size_t i = 0; i < length; i += maxBufferLength) {
              length -= mySocket->send(boost::asio::buffer(buffer + i, std::min(maxBufferLength, length)));
            }
          }
          virtual bool eof() const { return !mySocket->is_open(); }
          virtual JSObjectRef pause ( JSContextRef ctx, JSObjectRef thisObject ) {
            if (myPromise) {
              myState.store(Paused);
              return myPromise;
            } else {
              NX::Context * context = NX::Context::FromJsContext(ctx);
              return myPromise = NX::Object(context->toJSContext(),
                                            NX::Globals::Promise::createPromise(ctx, [=](NX::Context *, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
                                              myState.store(Paused);
                                              resolve(thisObject);
                                            }));
            }
          }
          virtual JSObjectRef reset ( JSContextRef ctx, JSObjectRef thisObject ) {
            if (myState != Paused)
              return pause(ctx, thisObject);
            else {
              return NX::Globals::Promise::resolve(ctx, thisObject);
            }
          }
          virtual JSObjectRef resume ( JSContextRef ctx, JSObjectRef thisObject );
          virtual State state() const { return myState; }



        private:
          NX::Scheduler * myScheduler;
          std::shared_ptr< boost::asio::ip::tcp::socket> mySocket;
          std::atomic<State> myState;
          NX::Object myPromise;
          boost::asio::ip::tcp::endpoint myEndpoint;
        };

        class UDPSocket: public virtual Socket {
        public:
          UDPSocket ( NX::Scheduler * scheduler, const std::shared_ptr< boost::asio::ip::udp::socket> & socket):
            myScheduler(scheduler), mySocket(socket)
          {
          }
          virtual ~UDPSocket()  {}
        private:
          static const JSClassDefinition Class;
          static const JSStaticValue Properties[];
          static const JSStaticFunction Methods[];

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef* exception);

          static void Finalize(JSObjectRef object) { }

        public:
          static JSClassRef createClass(NX::Context * context);
          static JSObjectRef getConstructor(NX::Context * context);

          static NX::Classes::IO::Devices::UDPSocket * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::UDPSocket*>(NX::Classes::Base::FromObject(obj));
          }

          virtual std::size_t available() const { return mySocket->available(); }
          virtual void cancel() { mySocket->cancel(); }
          virtual void close() { mySocket->close(); }
          virtual JSObjectRef connect (JSContextRef ctx, JSObjectRef thisObject, const std::string & address, const std::string & port, JSValueRef * exception);
          virtual JSObjectRef bind( JSContextRef ctx, JSObjectRef thisObject, const std::string & protocol, unsigned int port, JSValueRef * exception );
          virtual bool deviceReady() const { return mySocket->is_open(); }
          virtual std::size_t maxWriteBufferSize() const { return 65507; }
          virtual std::size_t recommendedWriteBufferSize() const { return maxWriteBufferSize() * 0.5; }
          virtual void deviceWrite ( const char * buffer, std::size_t length ) {
            boost::system::error_code ec;
            const std::size_t maxBufferLength = maxWriteBufferSize();
            for(std::size_t i = 0; i < length; i += maxBufferLength) {
              length -= mySocket->send_to(boost::asio::buffer(buffer + i, std::min(maxBufferLength, length)), myEndpoint, 0, ec);
              if (ec) {
                throw std::runtime_error(ec.message());
              }
            }
          }
          virtual bool eof() const { return !mySocket->is_open(); }
          virtual JSObjectRef pause ( JSContextRef ctx, JSObjectRef thisObject ) {
            if (myPromise) {
              myState.store(Paused);
              return myPromise;
            } else {
              NX::Context * context = NX::Context::FromJsContext(ctx);
              return myPromise = NX::Object(context->toJSContext(),
                NX::Globals::Promise::createPromise(ctx, [=](NX::Context *, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
                  myState.store(Paused);
                  resolve(thisObject);
                }));
            }
          }
          virtual JSObjectRef reset ( JSContextRef ctx, JSObjectRef thisObject ) {
            if (myState != Paused)
              return pause(ctx, thisObject);
            else {
              return NX::Globals::Promise::resolve(ctx, thisObject);
            }
          }
          virtual JSObjectRef resume ( JSContextRef ctx, JSObjectRef thisObject );
          virtual State state() const { return myState; }
        private:
          NX::Scheduler * myScheduler;
          std::shared_ptr< boost::asio::ip::udp::socket> mySocket;
          std::atomic<State> myState;
          boost::asio::ip::udp::endpoint myEndpoint;
          NX::Object myPromise;
        };

      }
    }
  }
}

#endif // CLASSES_IO_DEVICES_SOCKET_H
