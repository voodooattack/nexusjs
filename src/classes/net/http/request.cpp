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

JSObjectRef NX::Classes::Net::HTTP::Request::attach (JSContextRef ctx, JSObjectRef thisObject, JSObjectRef connection) {
  NX::Context * context = NX::Context::FromJsContext(ctx);
  NX::Object thisObj(context->toJSContext(), thisObject);
  NX::Object connectionObj(context->toJSContext(), connection);
  auto onChunkHeader = [=](std::uint64_t size, boost::beast::string_view extensions, boost::system::error_code& ec)
  {
    JSValueRef err = nullptr;
    if (ec) {
      auto msg = JSValueMakeString(context->toJSContext(), JSStringCreateWithUTF8CString(ec.message().c_str()));
      JSValueRef list[] = {msg};
      err = JSObjectMakeError(context->toJSContext(), 1, list, nullptr);
      JSValueRef arguments[] = {err};
      emitFastAndSchedule(context->toJSContext(), thisObj.value(), "error", 1, arguments, nullptr);
    }
    JSValueRef arguments[] = {
        JSValueMakeNumber(context->toJSContext(), size),
        JSValueMakeString(context->toJSContext(), JSStringCreateWithUTF8CString(extensions.to_string().c_str())),
        err
    };
    emitFastAndSchedule(context->toJSContext(), thisObj.value(), "header", ec ? 3 : 2, arguments, nullptr);
  };
  myReqParser.on_chunk_header(onChunkHeader);
  auto onChunkBody = [=](std::uint64_t remain, boost::beast::string_view body, boost::system::error_code& ec)
  {
    JSValueRef err = nullptr;
    if (ec)
    {
      auto msg = JSValueMakeString(context->toJSContext(), JSStringCreateWithUTF8CString(ec.message().c_str()));
      JSValueRef list[] = { msg };
      err = JSObjectMakeError(context->toJSContext(), 1, list, nullptr);
      JSValueRef arguments[] = { err };
      emitFastAndSchedule(context->toJSContext(), thisObj.value(), "error", 1, arguments, nullptr);
    }
    auto str = new std::string(body.begin(), body.end());
    JSValueRef arguments[] = {
        JSValueMakeNumber(context->toJSContext(), remain),
        JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(), (void*)str->c_str(), str->length(), [](void * data, void * pStr) {
          delete reinterpret_cast<std::string *>(pStr);
        }, str, nullptr),
        err
    };
    NX::Object promise(context->toJSContext(), emit(context->toJSContext(), thisObj.value(), "body", ec ? 3 : 2, arguments, nullptr));
    return JSValueToNumber(context->toJSContext(), promise.await(), nullptr);
  };
  myReqParser.on_chunk_body(onChunkBody);
  return NX::Globals::Promise::createPromise(context->toJSContext(),
    [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
      myConnection->addListener(context->toJSContext(), connection, "data",
        [=](JSContextRef ctx, std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef {
          context->nexus()->scheduler()->scheduleCoroutine([=]{
            try {
              auto exec = context->globalObject()->globalExec();
              auto gCtx = toRef(exec);
              JSC::JSLockHolder lock(exec);
              NX::Object buffer(gCtx, arguments[0]);
              auto * data = (const char *)JSObjectGetArrayBufferBytesPtr(gCtx, buffer, exception);
              std::size_t size = JSObjectGetArrayBufferByteLength(gCtx, buffer, exception);
              boost::system::error_code ec;
              myReqParser.put(boost::asio::const_buffers_1(data, size), ec);
              JSValueRef dataArgs[] { buffer };
              context->nexus()->scheduler()->scheduleTask([=]{
                emitFastAndSchedule(gCtx, thisObject, "data", 1, dataArgs, exception);
                if (myReqParser.is_header_done() && myReqParser.is_done()) {
                  NX::Object req(gCtx, thisObject);
                  std::string method = boost::to_string(myRequest.method());
                  req.set("method", NX::Value(gCtx, method).value());
                  unsigned major = myRequest.version() / 10;
                  unsigned minor = myRequest.version() % 10;
                  req.set("version", NX::Value(gCtx, major + "." + minor).value());
                  req.set("url", NX::Value(gCtx, myRequest.target().to_string()).value());
                  NX::Object headers(gCtx, JSObjectMakeArray(gCtx, 0, nullptr, exception));
                  for (const auto &i : myReqParser.get()) {
                    NX::Object header(gCtx);
                    header.set("name", NX::Value(gCtx, boost::to_string(i.name())).value());
                    header.set("value", NX::Value(gCtx, i.value().to_string()).value());
                    headers.push(header);
                  }
                  req.set("headers", headers.value());
                  emitFastAndSchedule(gCtx, thisObject, "end", 0, nullptr, exception);
                  resolve(thisObject);
                }
              });
              return JSValueMakeUndefined(gCtx);
            } catch(const std::exception & e) {
              return JSWrapException(ctx, e, exception);
            }
          });
          return JSValueMakeUndefined(ctx);
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
