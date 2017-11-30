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

#include <classes/net/http/response.h>
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
  myParser.on_chunk_header(onChunkHeader);
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
  myParser.on_chunk_body(onChunkBody);
  return NX::Globals::Promise::createPromise(context->toJSContext(),
    [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
      myConnection->addListener(context->toJSContext(), connection, "data",
        [=](JSContextRef ctx, std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef {
          auto exec = context->globalObject()->globalExec();
          auto gCtx = toRef(exec);
          JSObjectRef buffer = NX::Object(gCtx, arguments[0]);
          JSValueProtect(gCtx, buffer);
          auto * data = (const char *)JSObjectGetArrayBufferBytesPtr(gCtx, buffer, exception);
          std::size_t size = JSObjectGetArrayBufferByteLength(gCtx, buffer, exception);
          context->nexus()->scheduler()->scheduleCoroutine([=]{
            try {
              boost::system::error_code ec;
              myParser.put(boost::asio::const_buffers_1(data, size), ec);
              context->nexus()->scheduler()->scheduleTask([this, thisObject, buffer, resolve, reject, gCtx]{
                JSValueRef dataArgs[]{buffer};
                JSValueRef pException = nullptr;
                emitFastAndSchedule(gCtx, thisObject, "data", 1, dataArgs, &pException);
                JSValueUnprotect(gCtx, buffer);
                if (pException) {
                  return reject(pException);
                }
                if (myParser.is_header_done() && myParser.is_done()) {
                  try {
                    NX::Object req(gCtx, thisObject);
                    std::string method(boost::to_string(myParser.get().method()));
                    auto response = dynamic_cast<HTTP::Response*>(myConnection->res());
                    if (myParser.is_keep_alive()) {
                      myConnection->socket()->set_option(boost::asio::socket_base::keep_alive());
                      response->res().keep_alive(true);
                    }
                    response->res().version(version());
                    req.set("method", NX::Value(gCtx, method).value());
                    int major = myParser.get().version() / 10;
                    int minor = myParser.get().version() % 10;
                    std::string version(boost::to_string(major) + "." + boost::to_string(minor));
                    req.set("version", NX::Value(gCtx, version).value());
                    req.set("url", NX::Value(gCtx, myParser.get().target().to_string()).value());
                    NX::Object headers(gCtx, JSObjectMakeArray(gCtx, 0, nullptr, nullptr));
                    for (const auto &i : myParser.get()) {
                      NX::Object header(gCtx);
                      header.set("name", NX::Value(gCtx, boost::to_string(i.name())).value());
                      header.set("value", NX::Value(gCtx, i.value().to_string()).value());
                      headers.push(header);
                    }
                    req.set("headers", headers.value());
                    emitFastAndSchedule(gCtx, thisObject, "end", 0, nullptr, &pException);
                    if (pException)
                      return reject(pException);
                    resolve(thisObject);
                  } catch( const std::exception & e) {
                    JSValueUnprotect(gCtx, buffer);
                    reject(NX::Object(gCtx, e));
                  }
                }
              });
            } catch(const std::exception & e) {
              context->nexus()->scheduler()->scheduleTask([this, e, thisObject, buffer, resolve, reject, gCtx]{
                JSValueUnprotect(gCtx, buffer);
                reject(NX::Object(gCtx, e));
              });
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
