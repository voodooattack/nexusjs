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

#include "classes/net/htcommon/response.h"

const JSClassDefinition NX::Classes::Net::HTCommon::Response::Class {
  0, kJSClassAttributeNone, "Response", nullptr, NX::Classes::Net::HTCommon::Response::Properties,
  NX::Classes::Net::HTCommon::Response::Methods, nullptr
};

const JSStaticValue NX::Classes::Net::HTCommon::Response::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Net::HTCommon::Response::Methods[] {
  { "status", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
    auto res = NX::Classes::Net::HTCommon::Response::FromObject(thisObject);
    if (!res)
      throw NX::Exception("HTCommon::Response does not implement status()");
    if (argumentCount == 0) {
      // No arguments, return the status
      return JSValueMakeNumber(ctx, res->status());
    } else {
      if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
        NX::Value message(ctx, "status must be a number");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return thisObject;
      }
      double intPart = 0, status = NX::Value(ctx, arguments[0]).toNumber();
      if (std::modf(status, &intPart) != 0) {
        NX::Value message(ctx, "status must be an unsigned integer");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return thisObject;
      }
      try {
        res->status((unsigned) status);
      } catch (const std::exception & e) {
        *exception = NX::Object(ctx, e);
        return thisObject;
      }
    }
    return thisObject;
  }, 0 },
  { "set", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
    auto res = NX::Classes::Net::HTCommon::Response::FromObject(thisObject);
    if (!res)
      throw NX::Exception("HTCommon::Response does not implement set()");
    if (argumentCount != 2) {
      NX::Value message(ctx, "set() requires exactly 2 string parameters: header and value");
      JSValueRef args[] { message.value(), nullptr };
      *exception = JSObjectMakeError(ctx, 1, args, nullptr);
      return thisObject;
    } else {
      if (JSValueGetType(ctx, arguments[0]) != kJSTypeString) {
        NX::Value message(ctx, "header name must be a string");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return thisObject;
      }
      try {
        res->set(NX::Value(ctx, arguments[0]).toString(), NX::Value(ctx, arguments[1]).toString());
      } catch (const std::exception & e) {
        *exception = NX::Object(ctx, e);
        return thisObject;
      }
    }
    return thisObject;
  }, 0 },
  { nullptr, nullptr, 0 }
};
