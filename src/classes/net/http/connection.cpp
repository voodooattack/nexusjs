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

#include "classes/net/http/connection.h"

const JSClassDefinition NX::Classes::Net::HTTP::Connection::Class {
  0, kJSClassAttributeNone, "HTTPConnection", nullptr, NX::Classes::Net::HTTP::Connection::Properties,
  NX::Classes::Net::HTTP::Connection::Methods, nullptr
};

const JSStaticValue NX::Classes::Net::HTTP::Connection::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Net::HTTP::Connection::Methods[] {
  { nullptr, nullptr, 0 }
};

JSObjectRef NX::Classes::Net::HTTP::Connection::start(NX::Context * context, JSObjectRef thisObject)
{
  JSValueProtect(context->toJSContext(), thisObject);
  return NX::Globals::Promise::createPromise(context->toJSContext(),
    [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
      myRequest.initialize();
      myResponse.initialize();
      myReqLoader.initialize(&myRequest);
      myResLoader.initialize(&myResponse);
      addListener(context->toJSContext(), thisObject, "data",
        [=](JSContextRef ctx, std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef {
          JSValueUnprotect(context->toJSContext(), thisObject);
          try {
            NX::Object buffer(ctx, arguments[0]);
            const char * data = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, buffer, exception);
            std::size_t size = JSObjectGetArrayBufferByteLength(ctx, buffer, exception);
            myReqLoader.feed(std::string(data, data + size));
            if (myReqLoader.ready()) {
              myReqLoader.finalize();
              NX::Object req(context->toJSContext());
              req.set("method", NX::Value(ctx, myRequest.method).value());
              req.set("url", NX::Value(ctx, myRequest.url.to_string()).value());
              NX::Object thisObj(context->toJSContext(), thisObject);
              thisObj.set("request", req.value());
              resolve(thisObject);
            }
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
        });
      resume(context->toJSContext(), thisObject);
    });
}
