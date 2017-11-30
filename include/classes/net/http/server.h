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


#ifndef CLASSES_NET_HTTP_SERVER_H
#define CLASSES_NET_HTTP_SERVER_H

#include "classes/net/tcp/acceptor.h"

namespace NX {
  namespace Classes {
    namespace Net {
      namespace HTTP {
        class Server: public NX::Classes::Net::TCP::Acceptor {
          Server (NX::Scheduler * scheduler, const std::shared_ptr< boost::asio::ip::tcp::acceptor> & acceptor):
            Acceptor(scheduler, acceptor)
          {
          }

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef* exception)
          {
            NX::Context * context = NX::Context::FromJsContext(ctx);
            try {
              return JSObjectMake(ctx, createClass(context), dynamic_cast<NX::Classes::Base*>(
                new Server(context->nexus()->scheduler(), std::make_shared<boost::asio::ip::tcp::acceptor>(
                  *context->nexus()->scheduler()->service()))));
            } catch(const std::exception & e) {
              JSWrapException(ctx, e, exception);
              return JSObjectMake(ctx, nullptr, nullptr);
            }
          }

          static void Finalize(JSObjectRef object) { }

        public:
          virtual ~Server() = default;

          static NX::Classes::Net::HTTP::Server * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::Net::HTTP::Server*>(NX::Classes::Base::FromObject(obj));
          }

          static JSObjectRef getConstructor(NX::Context * context) {
            return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::Net::HTTP::Server::Constructor);
          }

          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::Net::HTTP::Server::Class;
            def.parentClass = NX::Classes::Net::TCP::Acceptor::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }

          void handleAccept(NX::Context* context, JSObjectRef thisObject, const std::shared_ptr<boost::asio::ip::tcp::socket> & socket) override;

          static const JSClassDefinition Class;
          static const JSStaticFunction Methods[];
          static const JSStaticValue Properties[];

        };
      }
    }
  }
}

#endif
