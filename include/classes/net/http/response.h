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
#include <boost/asio/buffer.hpp>

#include "classes/net/http/connection.h"
#include "classes/net/htcommon/response.h"

namespace NX {
  namespace Classes {
    namespace Net {
      namespace HTTP {
        class Request;
        class Response: public NX::Classes::Net::HTCommon::Response {
        public:
          explicit Response (NX::Classes::Net::HTTP::Connection * connection, bool continuation);

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

          unsigned status() const override { return myStatus; }

          void status(unsigned status) override {
            if (myHeadersSentFlag)
              throw NX::Exception("headers already sent");
            myStatus = status;
          }

          void set(const std::string & name, const std::string & value) override {
            if (myHeadersSentFlag)
              throw NX::Exception("headers already sent");
            myRes->set(boost::beast::string_view(name), boost::beast::string_param(value));
          }

          bool deviceReady() const override { return myConnection->deviceReady(); }
          bool deviceOpen() const override { return myConnection->deviceOpen(); }
          std::size_t deviceWrite ( const char * buffer, std::size_t length ) override;

          void send(JSContextRef context, JSValueRef body) override;

          std::size_t maxWriteBufferSize() const override { return UINT16_MAX; }

        public:

          struct Writer {

            explicit Writer(Net::HTTP::Connection * connection): myConnection(connection) { }

            boost::asio::io_service & get_io_service() {
              return *myConnection->scheduler()->service();
            }

            template<class ConstBufferSequence>
            std::size_t write_some(ConstBufferSequence const & sequence, boost::system::error_code & ec) {
              try {
                return write_some(sequence);
              } catch (const std::exception & e) {
                if (myConnection->error())
                  ec = myConnection->error();
                else {
                  ec.assign(errno, boost::system::system_category());
                }
                return 0;
              }
            }

            template<class ConstBufferSequence>
            std::size_t write_some(ConstBufferSequence const & sequence) {
              std::size_t written = 0;
              for (auto const & buffer : sequence) {
                std::size_t bufSize = boost::asio::buffer_size(buffer);
                if (bufSize)
                  myConnection->deviceWrite((const char *)boost::asio::detail::buffer_cast_helper(buffer), bufSize);
                written += bufSize;
              }
              if (written == 0)
                myConnection->deviceWrite(nullptr, 0);
              return written;
            }

          protected:
            Net::HTTP::Connection * myConnection;
          };

        protected:
          typedef boost::beast::http::dynamic_body Body;
          typedef boost::beast::http::response_serializer<Body> Serializer;
          typedef boost::beast::http::response<Body> BeastResponse;

          friend class HTTP::Request;

          BeastResponse & res() { return *myRes; }

          NX::Classes::Net::HTTP::Connection * myConnection;
          std::unique_ptr<BeastResponse> myRes;
          std::unique_ptr<Writer> myWriter;
          std::atomic_bool myHeadersSentFlag;
          unsigned int myStatus;
          bool myContinuationFlag;
        };
      }
    }
  }
}

#endif
