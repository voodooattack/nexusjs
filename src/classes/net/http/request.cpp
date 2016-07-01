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

#include "classes/net/http/request.h"

JSObjectRef NX::Classes::Net::HTTP::Request::attach (JSContextRef ctx, JSObjectRef thisObject) {
  NX::Context * context = NX::Context::FromJsContext(ctx);
  return NX::Globals::Promise::createPromise(context->toJSContext(),
    [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
      myConnection->addListener(context->toJSContext(), thisObject, "data",
        [=](JSContextRef ctx, std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef {
          JSValueUnprotect(context->toJSContext(), thisObject);
          try {
            NX::Object buffer(ctx, arguments[0]);
            const char * data = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, buffer, exception);
            std::size_t size = JSObjectGetArrayBufferByteLength(ctx, buffer, exception);
            myReqLoader.feed(std::string(data, data + size));
            JSValueRef dataArgs[] { buffer };
            emitFastAndSchedule(ctx, thisObject, "data", 1, dataArgs, exception);
            if (myReqLoader.ready()) {
              myReqLoader.finalize();
              emitFastAndSchedule(ctx, thisObject, "end", 0, nullptr, exception);
              NX::Object req(context->toJSContext(), thisObject);
              req.set("method", NX::Value(ctx, myRequest.method).value());
              req.set("version", NX::Value(ctx, myRequest.versionStr(myRequest.version)).value());
              req.set("url", NX::Value(ctx, myRequest.url.to_string()).value());
              NX::Object headers(ctx);
              for(const auto & header : myRequest.headers)
                headers.set(header.first, NX::Value(ctx, header.second).value());
              req.set("headers", headers.value());
              resolve(thisObject);
            }
            return JSValueMakeUndefined(ctx);
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
        });
      NX::Object resumePromise(ctx, resume(context->toJSContext(), thisObject));
      resumePromise.then([=](JSContextRef ctx, JSValueRef value, JSValueRef * exception) {
        return value;
      }, [=](JSContextRef ctx, JSValueRef error, JSValueRef * exception) {
        JSValueRef errArguments[] { error };
        emitFastAndSchedule(ctx, thisObject, "error", 1, errArguments, exception);
        return JSValueMakeUndefined(ctx);
      });
    });
}
