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

namespace NX {
  namespace Classes {
    namespace IO {
      namespace Devices {
        class Socket: public virtual BidirectionalDevice
        {
        protected:
          Socket(NX::Scheduler * scheduler, const std::shared_ptr<boost::asio::socket_base> & socket): myScheduler(scheduler), mySocket(socket) {}
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

          static NX::Classes::IO::Devices::Socket * FromObject(JSObjectRef object) {
            NX::Classes::IO::Device * context = reinterpret_cast<NX::Classes::IO::Device *>(JSObjectGetPrivate(object));
            return dynamic_cast<NX::Classes::IO::Devices::Socket*>(context);
          }

          virtual std::shared_ptr<boost::asio::socket_base> socket() { return mySocket; }

          virtual std::size_t available() const;
          virtual void close() = 0;
          virtual void cancel() = 0;
          virtual void connect(const std::string & endpoint) = 0;
          virtual void bind(const std::string & endpoint) = 0;

        private:
          NX::Scheduler * myScheduler;
          std::shared_ptr<boost::asio::socket_base> mySocket;
        };

        class TCPSocket: public virtual Socket {
        public:
          TCPSocket ( Scheduler * scheduler, const std::shared_ptr< boost::asio::ip::tcp::socket> & socket ): Socket(scheduler, socket) {}
          virtual ~TCPSocket()  {}

        };
      }
    }
  }
}

#endif // CLASSES_IO_DEVICES_SOCKET_H
