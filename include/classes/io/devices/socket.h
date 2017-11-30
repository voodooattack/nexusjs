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
#include <utility>

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

          virtual NX::Scheduler * scheduler() const = 0;

          virtual JSObjectRef connect(JSContextRef ctx, JSObjectRef thisObject, const std::string & address, const std::string & port, JSValueRef * exception) = 0;

        };

        class TCPSocket: public virtual Socket {
        public:
          TCPSocket ( NX::Scheduler * scheduler, std::shared_ptr<boost::asio::ip::tcp::socket> socket):
            myScheduler(scheduler), mySocket(std::move(socket))
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

          std::size_t available() const override { return mySocket->available(); }
          void cancel() override { mySocket->cancel(); }
          void close() override { mySocket->close(); }
          JSObjectRef connect (JSContextRef ctx, JSObjectRef thisObject, const std::string & address,
                                       const std::string & port, JSValueRef * exception) override;
          bool deviceReady() const override { return mySocket->is_open(); }
          std::size_t maxWriteBufferSize() const override { return UINT64_MAX; }
          std::size_t recommendedWriteBufferSize() const override { return maxWriteBufferSize(); }
          bool eof() const override { return !mySocket->is_open(); }
          void deviceWrite ( const char * buffer, std::size_t length ) override;
          JSObjectRef pause ( JSContextRef ctx, JSObjectRef thisObject ) override;
          JSObjectRef reset ( JSContextRef ctx, JSObjectRef thisObject ) override;
          JSObjectRef resume ( JSContextRef ctx, JSObjectRef thisObject ) override;

          State state() const override { return myState; }

          NX::Scheduler * scheduler() const override { return myScheduler; }

        private:
          NX::Scheduler * myScheduler;
          std::shared_ptr< boost::asio::ip::tcp::socket> mySocket;
          std::atomic<State> myState;
          NX::Object myPromise;
          boost::asio::ip::tcp::endpoint myEndpoint;
        };

        class UDPSocket: public virtual Socket {
        public:
          UDPSocket ( NX::Scheduler * scheduler, std::shared_ptr<boost::asio::ip::udp::socket> socket):
            myScheduler(scheduler), mySocket(std::move(socket))
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

          std::size_t available() const override { return mySocket->available(); }
          void cancel() override { mySocket->cancel(); }
          void close() override { mySocket->close(); }
          JSObjectRef connect (JSContextRef ctx, JSObjectRef thisObject, const std::string & address,
                               const std::string & port, JSValueRef * exception) override;
          virtual JSObjectRef bind( JSContextRef ctx, JSObjectRef thisObject,
                                    const std::string & protocol, unsigned int port, JSValueRef * exception );
          bool deviceReady() const override { return mySocket->is_open(); }
          std::size_t maxWriteBufferSize() const override { return 65507; }
          std::size_t recommendedWriteBufferSize() const override { return maxWriteBufferSize(); }

          void deviceWrite ( const char * buffer, std::size_t length ) override {
            boost::system::error_code ec;
            const std::size_t maxBufferLength = maxWriteBufferSize();
            std::size_t remaining = length;
            for(std::size_t i = 0; i < length; i += maxBufferLength) {
              remaining -= mySocket->send_to(boost::asio::buffer(buffer + i, std::min(maxBufferLength, remaining)), myEndpoint, 0, ec);
              if (ec) {
                throw NX::Exception(ec.message());
              }
            }
          }

          bool eof() const override { return !mySocket->is_open(); }

          JSObjectRef pause ( JSContextRef ctx, JSObjectRef thisObject ) override {
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

          JSObjectRef reset ( JSContextRef ctx, JSObjectRef thisObject ) override {
            if (myState != Paused)
              return pause(ctx, thisObject);
            else {
              return NX::Globals::Promise::resolve(ctx, thisObject);
            }
          }

          JSObjectRef resume ( JSContextRef ctx, JSObjectRef thisObject ) override;
          State state() const override { return myState; }
          NX::Scheduler * scheduler() const override { return myScheduler; }

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
