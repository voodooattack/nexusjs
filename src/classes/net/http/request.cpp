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

JSValueRef NX::Classes::Net::HTTP::Request::attach(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef connection) {
  NX::Context *context = NX::Context::FromJsContext(ctx);
  NX::Object thisObj(context->toJSContext(), thisObject);
  NX::Object connectionObj(context->toJSContext(), connection);
//  if (!myHeaderParsedFlag) {
//    myHeaderParsedFlag.store(true);
//    auto onChunkHeader = [=](std::uint64_t size, boost::beast::string_view extensions, boost::system::error_code &ec) {
//      JSValueRef err = nullptr;
//      if (ec && ec != boost::system::errc::operation_canceled) {
//        auto msg = JSValueMakeString(context->toJSContext(), JSStringCreateWithUTF8CString(ec.message().c_str()));
//        JSValueRef args[] = {msg};
//        err = JSObjectMakeError(context->toJSContext(), 1, args, nullptr);
//        JSValueRef arguments[] = {err};
//        emitFastAndSchedule(context->toJSContext(), thisObj.value(), "error", 1, arguments, nullptr);
//      }
//      JSValueRef args[] = {
//        JSValueMakeNumber(context->toJSContext(), size),
//        JSValueMakeString(context->toJSContext(), JSStringCreateWithUTF8CString(extensions.to_string().c_str())),
//        err
//      };
//      emitFastAndSchedule(context->toJSContext(), thisObj.value(), "header", ec && ec != boost::system::errc::operation_canceled ? 3 : 2, args, nullptr);
//    };
//    myParser->on_chunk_header(onChunkHeader);
//    auto onChunkBody = [=](std::uint64_t remain, boost::beast::string_view body, boost::system::error_code &ec) {
//      JSValueRef err = nullptr;
//      if (ec && ec != boost::system::errc::operation_canceled) {
//        auto msg = JSValueMakeString(context->toJSContext(), JSStringCreateWithUTF8CString(ec.message().c_str()));
//        JSValueRef args[] = {msg};
//        err = JSObjectMakeError(context->toJSContext(), 1, args, nullptr);
//        JSValueRef arguments[] = {err};
//        emitFastAndSchedule(context->toJSContext(), thisObj.value(), "error", 1, arguments, nullptr);
//      }
//      auto str = new std::string(body.begin(), body.end());
//      JSValueRef args[] = {
//        JSValueMakeNumber(context->toJSContext(), remain),
//        JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(), (void *) str->c_str(), str->length(),
//                                               [](void *data, void *pStr) {
//                                                 delete reinterpret_cast<std::string *>(pStr);
//                                               }, str, nullptr),
//        err
//      };
//      NX::Object promise(context->toJSContext(),
//                         emit(context->toJSContext(), thisObj.value(), "body", ec &&  ec != boost::system::errc::operation_canceled ? 3 : 2, args, nullptr));
//      return JSValueToNumber(context->toJSContext(), promise.await(), nullptr);
//    };
//    myParser->on_chunk_body(onChunkBody);
//  }
  NX::Globals::Promise::Executor executor = [=](JSContextRef ctx, ResolveRejectHandler resolve,
                                                ResolveRejectHandler reject) {
    myConnection->addListener(context->toJSContext(), connectionObj, "data",
                              [=](JSContextRef ctx, std::size_t argumentCount, const JSValueRef arguments[],
                                  JSValueRef *exception) -> JSValueRef {
                                NX::Object buffer(ctx, arguments[0]);
                                auto *data = (const char *) JSObjectGetArrayBufferBytesPtr(ctx, buffer, exception);
                                std::size_t size = JSObjectGetArrayBufferByteLength(ctx, buffer, exception);
                                try {
                                  boost::system::error_code &ec = error();
                                  myParser->put(boost::asio::const_buffers_1(data, size), ec);
                                  if (ec) {
                                    if (!myHeaderParsedFlag) {
                                      if (ec == boost::system::errc::operation_canceled) {
                                        resolve(context->toJSContext(), thisObj);
                                      } else {
                                        reject(context->toJSContext(), NX::Object(context->toJSContext(), ec));
                                      }
                                      return JSValueMakeUndefined(ctx);
                                    }
                                  }
                                  if (!myHeaderParsedFlag) {
                                    if (myParser->is_header_done()) {
                                      myHeaderParsedFlag.store(true);
                                      try {
                                        myConnection->keepAlive(myParser->is_keep_alive());
                                        NX::Object req(ctx, thisObj);
                                        std::string method(boost::to_string(myParser->get().method()));
                                        auto response = dynamic_cast<HTTP::Response *>(myConnection->res());
                                        if (myParser->is_keep_alive()) {
                                          myConnection->socket()->set_option(
                                            boost::asio::socket_base::keep_alive(true));
                                          response->res().keep_alive(true);
                                        }
                                        response->res().version(version());
                                        req.set("method", NX::Value(ctx, method).value());
                                        int major = myParser->get().version() / 10;
                                        int minor = myParser->get().version() % 10;
                                        std::string version(boost::to_string(major) + "." + boost::to_string(minor));
                                        req.set("version", NX::Value(ctx, version).value());
                                        req.set("url", NX::Value(ctx, myParser->get().target().to_string()).value());
                                        NX::Object headers(ctx, JSObjectMakeArray(ctx, 0, nullptr, nullptr));
                                        for (const auto &i : myParser->get()) {
                                          NX::Object header(ctx);
                                          header.set("name", NX::Value(ctx, boost::to_string(i.name())).value());
                                          header.set("value", NX::Value(ctx, i.value().to_string()).value());
                                          headers.push(header);
                                        }
                                        req.set("headers", headers.value());
                                        resolve(ctx, thisObj);
                                      } catch (const std::exception &e) {
                                        reject(ctx, NX::Object(ctx, e));
                                      }
                                    }
                                  }
                                  if (myParser->is_done()) {
                                    emitFast(context->toJSContext(), thisObj, "end", 0, nullptr, nullptr);
                                  } else {
                                    JSValueRef dataArgs[]{buffer};
                                    JSValueRef pException = nullptr;
                                    emitFast(ctx, thisObj, "data", 1, dataArgs, &pException);
                                    if (pException) {
                                      reject(context->toJSContext(), pException);
                                    }
                                  }
                                } catch (const std::exception &e) {
                                  reject(context->toJSContext(), NX::Object(ctx, e));
                                }
                                return JSValueMakeUndefined(ctx);
                              });
    myConnection->addOnceListener(context->toJSContext(), connectionObj, "error",
                              [=](JSContextRef ctx, std::size_t argumentCount, const JSValueRef arguments[],
                                  JSValueRef *exception) -> JSValueRef {
                                emitFast(ctx, thisObj, "error", argumentCount, arguments, exception);
                                reject(ctx, arguments[0]);
                                return JSValueMakeUndefined(ctx);
                              });
    NX::Object resumePromise(ctx, resume(context->toJSContext(), thisObj));
    resumePromise.then([=](JSContextRef ctx, JSValueRef value, JSValueRef *exception) {
                         return thisObj;
                       },
                       [=](JSContextRef ctx, JSValueRef error, JSValueRef *exception) {
                         JSValueRef errArguments[]{error};
                         emitFast(ctx, thisObj, "error", 1, errArguments, exception);
                         return JSValueMakeUndefined(ctx);
                       });
  };
  return NX::Globals::Promise::createPromise(context->toJSContext(), executor);
}
