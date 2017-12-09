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


#ifndef CLASSES_NET_HTCOMMON_CONNECTION_H
#define CLASSES_NET_HTCOMMON_CONNECTION_H

#include <JavaScriptCore/JSValueRef.h>
#include <JavaScriptCore/JSObjectRef.h>

#include "classes/io/devices/socket.h"
#include "globals/promise.h"

namespace NX {
  namespace Classes {
    namespace Net {
      namespace HTCommon {
        class Request;
        class Response;
        class Connection: public NX::Classes::IO::Devices::TCPSocket {
        public:
          Connection (NX::Scheduler * scheduler, std::shared_ptr< boost::asio::ip::tcp::socket> socket):
            TCPSocket(scheduler, socket), mySocket(std::move(socket)), myThisObject(), myContext(nullptr)
          {
          }

        public:
          virtual ~Connection() {}

          static NX::Classes::Net::HTCommon::Connection * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::Net::HTCommon::Connection*>(NX::Classes::Base::FromObject(obj));
          }

          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::Net::HTCommon::Connection::Class;
            def.parentClass = NX::Classes::IO::Devices::TCPSocket::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }

          virtual JSObjectRef start(NX::Context * context, JSObjectRef thisObject, bool continuation) {
            myContext = context;
            myThisObject = NX::Object(context->toJSContext(), thisObject);
            return NX::Globals::Promise::resolve(context->toJSContext(), thisObject);
          }

          virtual NX::Classes::Net::HTCommon::Response * res() const = 0;
          virtual NX::Classes::Net::HTCommon::Request * req() const = 0;

          static const JSClassDefinition Class;
          static const JSStaticFunction Methods[];
          static const JSStaticValue Properties[];

          NX::Context * context() const { return myContext; }
          JSObjectRef thisObject() const { return myThisObject; }

          void close() override {
            NX::Classes::IO::Devices::TCPSocket::close();
            emitFastAndSchedule(myContext->toJSContext(), myThisObject, "close", 0, nullptr, nullptr);
            myThisObject.clear();
          }

          std::shared_ptr< boost::asio::ip::tcp::socket> socket() const { return mySocket; }

        private:
          std::shared_ptr< boost::asio::ip::tcp::socket> mySocket;
          NX::Object myThisObject;
          NX::Context * myContext;
        };
      }
    }
  }
}

#endif
