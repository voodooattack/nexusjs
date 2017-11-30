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

#include "nexus.h"
#include "globals/promise.h"

#include "promise.js.inc"

JSValueRef NX::Globals::Promise::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (auto Promise = context->getGlobal("Promise"))
      return Promise;
  JSValueRef Promise = context->evaluateScript(std::string(promise_js, promise_js + promise_js_len),
                                              nullptr, "promise.js", 1, exception);
  JSObjectRef promiseObject = JSValueToObject(context->toJSContext(), Promise, exception);
  if (!*exception)
    return context->setGlobal("Promise", promiseObject);
  return JSValueMakeUndefined(ctx);
}

JSValueRef NX::Globals::Promise::createPromise (JSContextRef ctx, JSObjectRef executor, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  JSObjectRef Promise = JSValueToObject(ctx, context->getGlobal("Promise"), exception);
  JSValueRef args[]{executor};
  return JSObjectCallAsConstructor(ctx, Promise, 1, args, exception);
}

JSObjectRef NX::Globals::Promise::createPromise (JSContextRef ctx, const NX::Globals::Promise::Executor & executor)
{
  NX::Context * context = Context::FromJsContext(ctx);
  JSValueRef exp = nullptr;
  JSObjectRef Promise = JSValueToObject(ctx, context->getOrInitGlobal("Promise"), &exp);
  if (exp) {
    throw NX::Exception(NX::Value(context->toJSContext(), exp).toString());
  }
  JSObjectRef thisObject = JSObjectMake(ctx, context->nexus()->genericClass(), new NX::Globals::Promise::Executor(executor));
    JSObjectRef jsExecutor = JSBindFunction(ctx, JSObjectMakeFunctionWithCallback(ctx, ScopedString("CreatePromiseExecutor"),
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
          size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
        NX::Globals::Promise::Executor executor = *reinterpret_cast<NX::Globals::Promise::Executor*>(JSObjectGetPrivate(thisObject));
        delete reinterpret_cast<NX::Globals::Promise::Executor*>(JSObjectGetPrivate(thisObject));
        if (!executor) {
          NX::Value message(ctx, "promise executor is null");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
          return JSValueMakeUndefined(ctx);
        }
        JSObjectRef resolve = JSValueToObject(ctx, arguments[0], nullptr);
        JSObjectRef reject = JSValueToObject(ctx, arguments[1], nullptr);
        try {
          JSValueProtect(context->toJSContext(), thisObject);
          JSValueProtect(context->toJSContext(), resolve);
          JSValueProtect(context->toJSContext(), reject);
          executor(context, [=](JSValueRef resolveValue) {
            JSValueRef args[] { resolveValue };
            JSValueRef exp = nullptr;
            JSObjectCallAsFunction(context->toJSContext(), resolve, nullptr, 1, args, &exp);
            if (exp) {
              JSValueRef rejectArgs[] { exp };
              JSValueRef rejExp = nullptr;
              JSObjectCallAsFunction(context->toJSContext(), reject, nullptr, 1, rejectArgs, &rejExp);
              if (rejExp)
                NX::Nexus::ReportException(context->toJSContext(), rejExp);
            }
            JSValueUnprotect(context->toJSContext(), resolve);
            JSValueUnprotect(context->toJSContext(), reject);
            JSValueUnprotect(context->toJSContext(), thisObject);
          }, [=](JSValueRef rejectValue) {
            JSValueRef args[] { rejectValue };
            JSValueRef exp = nullptr;
            JSObjectCallAsFunction(context->toJSContext(), reject, nullptr, 1, args, &exp);
            if (exp)
              NX::Nexus::ReportException(context->toJSContext(), exp);
            JSValueUnprotect(context->toJSContext(), resolve);
            JSValueUnprotect(context->toJSContext(), reject);
            JSValueUnprotect(context->toJSContext(), thisObject);
          });
        } catch (const std::exception & e) {
          JSValueRef args[] { NX::Object(context->toJSContext(), e) };
          JSObjectCallAsFunction(context->toJSContext(), reject, nullptr, 1, args, exception);
          JSValueUnprotect(context->toJSContext(), resolve);
          JSValueUnprotect(context->toJSContext(), reject);
          JSValueUnprotect(context->toJSContext(), thisObject);
        }
        return JSValueMakeUndefined(ctx);
      }), thisObject, 0, nullptr, nullptr);
  JSValueRef args[] { jsExecutor };
  JSObjectRef promise = JSObjectCallAsConstructor(context->toJSContext(), Promise, 1, args, &exp);
  if (exp) {
    throw NX::Exception(NX::Value(context->toJSContext(), exp).toString());
  }
  return promise;
}

JSObjectRef NX::Globals::Promise::all (JSContextRef ctx, const std::vector< JSValueRef > & promises)
{
  if (promises.empty())
    return Promise::resolve(ctx, JSObjectMakeArray(ctx, 0, nullptr, nullptr));
  NX::Context * context = Context::FromJsContext(ctx);
  JSValueRef exp = nullptr;
  JSObjectRef Promise = JSValueToObject(ctx, context->getOrInitGlobal("Promise"), &exp);
  JSObjectRef arr = NX::Object(ctx, promises);
  return NX::Object(ctx, NX::Object(ctx, Promise)["all"]->toObject()->call(Promise, {arr}, nullptr));
}

JSObjectRef NX::Globals::Promise::resolve (JSContextRef ctx, const JSValueRef value)
{
  NX::Context * context = Context::FromJsContext(ctx);
  JSValueRef exp = nullptr;
  JSObjectRef Promise = JSValueToObject(ctx, context->getOrInitGlobal("Promise"), &exp);
  return NX::Object(ctx, NX::Object(ctx, Promise)["resolve"]->toObject()->call(Promise, std::vector<JSValueRef> { value }, nullptr));
}

JSObjectRef NX::Globals::Promise::reject (JSContextRef ctx, const JSValueRef value)
{
  NX::Context * context = Context::FromJsContext(ctx);
  JSValueRef exp = nullptr;
  JSObjectRef Promise = JSValueToObject(ctx, context->getOrInitGlobal("Promise"), &exp);
  return NX::Object(ctx, NX::Object(ctx, Promise)["reject"]->toObject()->call(Promise, std::vector<JSValueRef> { value }, nullptr));
}

