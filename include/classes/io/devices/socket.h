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

          virtual JSObjectRef connect(JSContextRef ctx, JSObjectRef thisObject, const std::string & address, JSValueRef * exception) = 0;

        };

        class TCPSocket: public virtual Socket {
        public:
          TCPSocket ( NX::Scheduler * scheduler, const std::shared_ptr< boost::asio::ip::tcp::socket> & socket):
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

          static NX::Classes::IO::Devices::TCPSocket * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::TCPSocket*>(NX::Classes::Base::FromObject(obj));
          }


        private:
          NX::Scheduler * myScheduler;
          std::shared_ptr< boost::asio::ip::tcp::socket> mySocket;
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
          virtual JSObjectRef connect ( JSContextRef ctx, JSObjectRef thisObject, const std::string & address, JSValueRef * exception );
          virtual bool deviceReady() const { return mySocket->is_open(); }
          virtual void deviceWrite ( const char * buffer, std::size_t length ) {
            boost::system::error_code ec;
            mySocket->send(boost::asio::buffer(buffer, length), 0, ec);
            if (ec)
              throw std::runtime_error(ec.message());
          }
          virtual bool eof() const { return !mySocket->is_open(); }
          virtual void pause ( JSContextRef ctx, JSObjectRef thisObject ) { myState.store(Paused); }
          virtual void reset ( JSContextRef ctx, JSObjectRef thisObject ) { myState.store(Paused); }
          virtual JSObjectRef resume ( JSContextRef ctx, JSObjectRef thisObject );
          virtual State state() const { return myState; }
        private:
          NX::Scheduler * myScheduler;
          std::shared_ptr< boost::asio::ip::udp::socket> mySocket;
          std::atomic<State> myState;
          boost::asio::ip::udp::endpoint myEndpoint;
        };

      }
    }
  }
}

#endif // CLASSES_IO_DEVICES_SOCKET_H
