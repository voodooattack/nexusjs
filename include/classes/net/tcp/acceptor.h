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

#ifndef CLASSES_NET_TCP_ACCEPTOR_H
#define CLASSES_NET_TCP_ACCEPTOR_H

#include "nexus.h"
#include <boost/asio.hpp>

#include "classes/emitter.h"
#include "util.h"
#include "globals/promise.h"

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
  {
    namespace Net
    {
      namespace TCP {
        class Acceptor: public NX::Classes::Emitter {
        protected:
          Acceptor (NX::Scheduler * scheduler, const std::shared_ptr< boost::asio::ip::tcp::acceptor> & acceptor):
            myScheduler(scheduler), myAcceptor(acceptor)
          {
          }

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                        const JSValueRef arguments[], JSValueRef* exception)
          {
            NX::Context * context = NX::Context::FromJsContext(ctx);
            try {
              return JSObjectMake(ctx, createClass(context), dynamic_cast<NX::Classes::Base*>(
                new Acceptor(context->nexus()->scheduler(), std::shared_ptr<boost::asio::ip::tcp::acceptor>(
                  new boost::asio::ip::tcp::acceptor(*context->nexus()->scheduler()->service())))
              ));
            } catch(const std::exception & e) {
              JSWrapException(ctx, e, exception);
              return JSObjectMake(ctx, nullptr, nullptr);
            }
          }

          static void Finalize(JSObjectRef object) { }

        public:
          virtual ~Acceptor() {}

          static NX::Classes::Net::TCP::Acceptor * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::Net::TCP::Acceptor*>(NX::Classes::Base::FromObject(obj));
          }

          static JSObjectRef getConstructor(NX::Context * context) {
            return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::Net::TCP::Acceptor::Constructor);
          }

          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::Net::TCP::Acceptor::Class;
            def.parentClass = NX::Classes::Emitter::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }

          static const JSClassDefinition Class;
          static const JSStaticFunction Methods[];
          static const JSStaticValue Properties[];

        public:

          JSValueRef bind ( JSContextRef ctx, JSObjectRef thisObject, const std::string & addr, short unsigned int port, JSValueRef * exception );
          JSValueRef listen ( JSContextRef ctx, JSObjectRef thisObject, int maxConnections, JSValueRef * exception );

        protected:

          virtual void beginAccept(NX::Context* context, JSObjectRef thisObject);
          virtual void handleAccept(NX::Context* context, JSObjectRef thisObject, const std::shared_ptr<boost::asio::ip::tcp::socket> & socket);

          NX::Scheduler * scheduler() { return myScheduler; }
          std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor() { return myAcceptor; }

        private:
          NX::Scheduler * myScheduler;
          std::shared_ptr<boost::asio::ip::tcp::acceptor> myAcceptor;
        };
      }
    }
  }
}

#endif // CLASSES_NET_TCP_ACCEPTOR_H
