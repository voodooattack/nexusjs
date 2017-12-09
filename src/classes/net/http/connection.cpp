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

#include <utility>

#include "classes/net/htcommon/connection.h"
#include "classes/net/http/connection.h"
#include "classes/net/http/request.h"
#include "classes/net/http/response.h"

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

JSObjectRef NX::Classes::Net::HTTP::Connection::start(NX::Context * context, JSObjectRef thisObject, bool continuation)
{
  NX::Object thisObj(context->toJSContext(), thisObject);
  auto req = myReq = new NX::Classes::Net::HTTP::Request(this, continuation);
  auto res = myRes = new NX::Classes::Net::HTTP::Response(this, continuation);
  JSObjectRef reqObj = JSObjectMake(context->toJSContext(), NX::Classes::Net::HTTP::Request::createClass(context), req);
  JSObjectRef resObj = JSObjectMake(context->toJSContext(), NX::Classes::Net::HTTP::Response::createClass(context), res);
  thisObj.set("request", reqObj);
  thisObj.set("response", resObj);
  JSObjectSetProperty(context->toJSContext(), reqObj, ScopedString("connection"), thisObj,
                      kJSPropertyAttributeDontDelete | kJSPropertyAttributeDontEnum | kJSPropertyAttributeReadOnly, nullptr);
  JSObjectSetProperty(context->toJSContext(), resObj, ScopedString("connection"), thisObj,
                      kJSPropertyAttributeDontDelete | kJSPropertyAttributeDontEnum | kJSPropertyAttributeReadOnly, nullptr);
  NX::Object basePromise (context->toJSContext(), NX::Classes::Net::HTCommon::Connection::start(context, thisObject, continuation));
  return basePromise.then([=](JSContextRef ctx, JSValueRef value, JSValueRef * exception){
    return NX::Object(context->toJSContext(), NX::Globals::Promise::all(context->toJSContext(), std::vector<JSValueRef> {
      req->attach(context->toJSContext(), reqObj, thisObject),
      res->attach(context->toJSContext(), resObj, thisObject)
    })).then([=](JSContextRef, JSValueRef, JSValueRef *) {
      return thisObj;
    });
  });
}

NX::Classes::Net::HTTP::Connection::Connection(Scheduler *scheduler, Server *server, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
  : NX::Classes::Net::HTCommon::Connection(scheduler, std::move(socket)), myServer(server),
    myRes(nullptr), myReq(nullptr), myKeepAliveFlag(false), myThisObject()
{
}

void NX::Classes::Net::HTTP::Connection::notifyCompleted() {
  auto context = NX::Context::FromJsContext(myThisObject.context());
  if (!keepAlive()) {
    this->close();
  } else {
    myServer->handleAccept(context, myServer->thisObject(), socket(), true, error());
  }
  myThisObject.clear();
}
