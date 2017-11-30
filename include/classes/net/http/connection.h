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


#ifndef CLASSES_NET_HTTP_CONNECTION_H
#define CLASSES_NET_HTTP_CONNECTION_H

#include <JavaScript.h>

#include "classes/net/htcommon/connection.h"

namespace NX {
  namespace Classes {
    namespace Net {
      namespace HTTP {
        class Connection: public NX::Classes::Net::HTCommon::Connection {
        protected:
          Connection(NX::Scheduler * scheduler, const std::shared_ptr< boost::asio::ip::tcp::socket> & socket);

        public:
          virtual ~Connection() = default;

          static NX::Classes::Net::HTTP::Connection * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::Net::HTTP::Connection*>(NX::Classes::Base::FromObject(obj));
          }

          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::Net::HTTP::Connection::Class;
            def.parentClass = NX::Classes::Net::HTCommon::Connection::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }

          static JSObjectRef wrapSocket(NX::Context * context, const std::shared_ptr<boost::asio::ip::tcp::socket> & socket) {
            return JSObjectMake(context->toJSContext(), createClass(context), new Connection(context->nexus()->scheduler(), socket));
          }

          JSObjectRef start(NX::Context * context, JSObjectRef thisObject) override;
          NX::Classes::Net::HTCommon::Response * res() const override { return myRes; };
          NX::Classes::Net::HTCommon::Request * req() const override { return myReq; };

          static const JSClassDefinition Class;
          static const JSStaticFunction Methods[];
          static const JSStaticValue Properties[];

        protected:

          NX::Classes::Net::HTCommon::Response * myRes;
          NX::Classes::Net::HTCommon::Request * myReq;

        };
      }
    }
  }
}

#endif
