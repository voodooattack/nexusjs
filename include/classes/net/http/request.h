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


#ifndef CLASSES_NET_HTTP_REQUEST_H
#define CLASSES_NET_HTTP_REQUEST_H

#include <JavaScriptCore/API/JSObjectRef.h>
#include <boost/beast.hpp>

#include "classes/net/http/connection.h"
#include "classes/net/htcommon/request.h"

namespace NX {
  namespace Classes {
    namespace Net {
      namespace HTTP {
        class Request: public NX::Classes::Net::HTCommon::Request {
        public:
          explicit Request (NX::Classes::Net::HTTP::Connection * connection):
            HTCommon::Request(connection), myConnection(connection), myParser()
          {
          }

        public:
          ~Request() override = default;

          static NX::Classes::Net::HTCommon::Request * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::Net::HTCommon::Request*>(NX::Classes::Base::FromObject(obj));
          }

          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::Net::HTCommon::Request::Class;
            def.parentClass = NX::Classes::Net::HTCommon::Request::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }

          static const JSClassDefinition Class;
          static const JSStaticFunction Methods[];
          static const JSStaticValue Properties[];

          JSObjectRef attach (JSContextRef ctx, JSObjectRef thisObject, JSObjectRef connection) override;

          NX::Classes::Net::HTTP::Connection * connection() { return myConnection; }

          bool deviceReady() const override { return myConnection->deviceReady(); }
          bool eof() const override { return myConnection->eof(); }
          JSObjectRef pause ( JSContextRef ctx, JSObjectRef thisObject ) override { return myConnection->pause(ctx, thisObject); }
          JSObjectRef reset ( JSContextRef ctx, JSObjectRef thisObject ) override { return myConnection->reset(ctx, thisObject); }
          JSObjectRef resume ( JSContextRef ctx, JSObjectRef thisObject ) override { return myConnection->resume(ctx, thisObject); }
          State state() const override { return myConnection->state(); }

          unsigned int version() const { return myParser.get().version(); }
          bool keep_alive() const { return myParser.get().keep_alive(); }
          boost::beast::http::verb method() const { return myParser.get().method(); }

        protected:
          NX::Classes::Net::HTTP::Connection * myConnection;
          boost::beast::http::request_parser<boost::beast::http::dynamic_body> myParser;
        };
      }
    }
  }
}

#endif
