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

#include "value.h"
#include "context.h"
#include "object.h"
#include "util.h"
#include "nexus.h"
#include "classes/context.h"

JSClassRef NX::Classes::Context::createClass (NX::Context * context)
{
  return context->nexus()->defineOrGetClass(NX::Classes::Context::Class);
}

JSObjectRef NX::Classes::Context::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::Context::Constructor);
}

JSObjectRef NX::Classes::Context::Constructor (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                            const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef contextClass = createClass(context);
  if (argumentCount > 0 && JSValueGetType(ctx, arguments[0]) != kJSTypeObject) {
    NX::Value message(ctx, "argument must be an object");
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
  try {
    return JSObjectMake(ctx, contextClass, new NX::Classes::Context(context, argumentCount ? NX::Object(ctx, arguments[0]).value() : nullptr));
  } catch(const std::exception & e) {
    NX::Value message(ctx, e.what());
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  }
  return JSObjectMake(ctx, nullptr, nullptr);
}

NX::Classes::Context::Context(NX::Context * parent, JSObjectRef globalOverrides):
  myContext(nullptr)
{
  myContext.reset(new NX::Context(parent));
  if (globalOverrides)
  {
    JSValueProtect(parent->toJSContext(), globalOverrides);
    JSObjectRef globalObject = myContext->globalObject();
    JSPropertyNameArrayRef namesArray = JSObjectCopyPropertyNames(parent->toJSContext(), globalOverrides);
    unsigned int count = JSPropertyNameArrayGetCount(namesArray);
    for(int i = 0; i < count; i++) {
      JSStringRef propertyName = JSPropertyNameArrayGetNameAtIndex(namesArray, i);
      JSObjectSetProperty(myContext->toJSContext(),
                          globalObject,
                          propertyName,
                          JSObjectGetProperty(parent->toJSContext(), globalOverrides, propertyName, nullptr),
                          kJSPropertyAttributeNone,
                          nullptr
                         );
    }
    JSPropertyNameArrayRelease(namesArray);
    JSValueUnprotect(parent->toJSContext(), globalOverrides);
  }
}

JSValueRef NX::Classes::Context::eval(JSContextRef ctx, JSObjectRef thisObject, const std::string & source,
                                      const std::string & fileName, unsigned int lineNo, JSValueRef * exception)
{
  return myContext->evaluateScript(source, nullptr, fileName, lineNo, exception);
}

NX::Classes::Context::~Context() {

}

#include <iostream>

JSValueRef NX::Classes::Context::exports(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
  return myContext->exports();
}

const JSClassDefinition NX::Classes::Context::Class {
  0, kJSClassAttributeNone, "Context", nullptr, NX::Classes::Context::Properties,
  NX::Classes::Context::Methods, nullptr, NX::Classes::Context::Finalize
};

const JSStaticValue NX::Classes::Context::Properties[] {
  { "exports", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      NX::Classes::Context * thisContext = FromObject(object);
      if (!thisContext) {
        NX::Value message(ctx, "invalid `this` value");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      return thisContext->exports(ctx, object, propertyName, exception);
    }, nullptr, kJSPropertyAttributeReadOnly
  },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Context::Methods[] {
  { "eval", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::Context * thisContext = NX::Classes::Context::FromObject(thisObject);
        std::string source, fileName;
        unsigned int lineNo;
        source = NX::Value(ctx, arguments[0]).toString();
        if (argumentCount >= 2) {
          fileName = NX::Value(ctx, arguments[1]).toString();
        }
        if (argumentCount >= 3) {
          lineNo = NX::Value(ctx, arguments[2]).toNumber();
        }
        return thisContext->eval(ctx, nullptr, source, fileName, lineNo, exception);
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
