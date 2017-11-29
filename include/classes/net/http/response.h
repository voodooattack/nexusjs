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


#ifndef CLASSES_NET_HTTP_RESPONSE_H
#define CLASSES_NET_HTTP_RESPONSE_H

#include <boost/beast.hpp>

#include "classes/net/http/connection.h"
#include "classes/net/htcommon/response.h"

namespace NX {
  namespace Classes {
    namespace Net {
      namespace HTTP {
        class Request;
        class Response: public NX::Classes::Net::HTCommon::Response {
        public:
          Response (NX::Classes::Net::HTTP::Connection * connection);

        public:
          virtual ~Response() {}

          static NX::Classes::Net::HTCommon::Response * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::Net::HTCommon::Response*>(NX::Classes::Base::FromObject(obj));
          }

          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::Net::HTCommon::Response::Class;
            def.parentClass = NX::Classes::Net::HTCommon::Response::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }

          static const JSClassDefinition Class;
          static const JSStaticFunction Methods[];
          static const JSStaticValue Properties[];

          JSObjectRef attach(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef connection) override;

          NX::Classes::Net::HTTP::Connection * connection() { return myConnection; }

          bool deviceReady() const override { return myConnection->deviceReady(); }

          virtual void deviceWrite ( const char * buffer, std::size_t length ) {
            boost::system::error_code ec;
            if (buffer)
              myResParser.put(boost::asio::const_buffers_1(buffer, length), ec);
            else
              myResParser.put_eof(ec);
            if (ec) {
              throw NX::Exception(ec.message());
            }
          }

          virtual std::size_t maxWriteBufferSize() const { return 8192; }

        protected:
          NX::Classes::Net::HTTP::Connection * myConnection;
          boost::beast::http::response<boost::beast::http::dynamic_body> myResponse;
          boost::beast::http::response_parser<boost::beast::http::dynamic_body> myResParser;
        };
      }
    }
  }
}

#endif
