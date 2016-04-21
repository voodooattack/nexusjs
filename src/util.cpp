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

#include "util.h"
#include "scoped_string.h"
#include "value.h"

JSObjectRef NX::JSBindFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                           size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::ScopedString strBind("Function.__proto__.bind");
  JSValueRef bind = JSEvaluateScript(ctx, strBind, JSContextGetGlobalObject(ctx), nullptr, 0, exception);
  std::vector<JSValueRef> args;
  args.push_back(thisObject);
  for(int i = 0; i < argumentCount; i++)
    args.push_back(arguments[i]);
  return JSValueToObject(ctx,
                         JSObjectCallAsFunction(ctx, JSValueToObject(ctx, bind, exception), function, args.size(), &args[0], exception), exception);
}

JSObjectRef NX::JSCopyObjectShallow(JSContextRef source, JSContextRef dest, JSObjectRef object, JSValueRef * exception) {
  JSValueProtect(source, object);
  JSClassRef emptyClass = JSClassCreate(&kJSClassDefinitionEmpty);
  JSObjectRef ret = JSObjectMake(dest, emptyClass, nullptr);
  JSClassRelease(emptyClass);
  JSPropertyNameArrayRef namesArray = JSObjectCopyPropertyNames(source, object);
  unsigned int count = JSPropertyNameArrayGetCount(namesArray);
  for(int i = 0; i < count; i++) {
    JSStringRef propertyName = JSPropertyNameArrayGetNameAtIndex(namesArray, i);
    JSObjectSetProperty(dest,
                        ret,
                        propertyName,
                        JSObjectGetProperty(source, object, propertyName, nullptr),
                        kJSPropertyAttributeNone,
                        nullptr
    );
  }
  JSPropertyNameArrayRelease(namesArray);
  JSValueUnprotect(source, object);
  return ret;
}

JSValueRef NX::JSWrapException (JSContextRef ctx, const std::exception & e, JSValueRef * exception)
{
  NX::Value message(ctx, e.what());
  JSValueRef args[] { message.value(), nullptr };
  if (exception)
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  return JSValueMakeUndefined(ctx);
}
