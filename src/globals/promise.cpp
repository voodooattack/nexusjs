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
  if (JSObjectRef Promise = context->getGlobal("Promise"))
    return Promise;
  JSValueRef Promise = context->evaluateScript(std::string(promise_js, promise_js + promise_js_len),
                                              nullptr, "promise.js", 1, exception);
  JSObjectRef promiseObject = JSValueToObject(context->toJSContext(), Promise, exception);
  if (!*exception)
    return context->setGlobal("Promise", promiseObject);
  return JSValueMakeUndefined(ctx);
}

JSObjectRef NX::Globals::Promise::createPromise (JSContextRef ctx, JSObjectRef executor, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  JSObjectRef Promise = context->getOrInitGlobal("Promise");
  JSValueRef args[] { executor };
  return JSObjectCallAsConstructor(ctx, Promise, 1, args, exception);
}

JSObjectRef NX::Globals::Promise::createPromise (JSContextRef ctx, const NX::Globals::Promise::Executor & executor, bool scheduleAsCoroutine)
{
  NX::Context * context = Context::FromJsContext(ctx);
  JSObjectRef Promise = context->getOrInitGlobal("Promise");
  JSObjectRef thisObject = JSObjectMake(context->toJSContext(), context->nexus()->genericClass(), new NX::Globals::Promise::Executor(executor));
  JSObjectSetProperty(ctx, thisObject, ScopedString("scheduleAsCoroutine"), JSValueMakeBoolean(ctx, scheduleAsCoroutine), 0, nullptr);
  JSObjectRef jsExecutor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
          size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
        NX::Globals::Promise::Executor executor = *(NX::Globals::Promise::Executor*)JSObjectGetPrivate(thisObject);
        delete (NX::Globals::Promise::Executor*)JSObjectGetPrivate(thisObject);
        if (!executor) {
          NX::Value message(ctx, "promise executor is null");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
          return JSValueMakeUndefined(ctx);
        }
        JSObjectRef resolve = NX::Object(context->toJSContext(), arguments[0]);
        JSObjectRef reject = NX::Object(context->toJSContext(), arguments[1]);
        JSValueProtect(context->toJSContext(), thisObject);
        JSValueProtect(context->toJSContext(), resolve);
        JSValueProtect(context->toJSContext(), reject);
        bool scheduleAsCoroutine = JSValueToBoolean(ctx, JSObjectGetProperty(ctx, thisObject, ScopedString("scheduleAsCoroutine"), nullptr));
        boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
        try {
          if (scheduleAsCoroutine) {
            scheduler->scheduleCoroutine([=]() {
              executor([=](JSValueRef resolveValue) {
                JSValueProtect(context->toJSContext(), resolveValue);
                scheduler->scheduleTask([=] {
                  JSValueRef args[] { resolveValue };
                  JSObjectCallAsFunction(context->toJSContext(), resolve, nullptr, 1, args, nullptr);
                  JSValueUnprotect(context->toJSContext(), resolve);
                  JSValueUnprotect(context->toJSContext(), reject);
                  JSValueUnprotect(context->toJSContext(), resolveValue);
                });
              }, [=](JSValueRef rejectValue) {
                JSValueProtect(context->toJSContext(), rejectValue);
                scheduler->scheduleTask([=]{
                  JSValueRef args[] { rejectValue };
                  JSObjectCallAsFunction(context->toJSContext(), reject, nullptr, 1, args, nullptr);
                  JSValueUnprotect(context->toJSContext(), resolve);
                  JSValueUnprotect(context->toJSContext(), reject);
                  JSValueUnprotect(context->toJSContext(), rejectValue);
                });
              });
            });
          } else {
            executor([=](JSValueRef resolveValue) {
              JSValueRef args[] { resolveValue };
              JSValueRef exp = nullptr;
              JSObjectCallAsFunction(context->toJSContext(), resolve, nullptr, 1, args, nullptr);
              JSValueUnprotect(context->toJSContext(), resolve);
              JSValueUnprotect(context->toJSContext(), reject);
            }, [=](JSValueRef rejectValue) {
              JSValueRef args[] { rejectValue };
              JSValueRef exp = nullptr;
              JSObjectCallAsFunction(context->toJSContext(), reject, nullptr, 1, args, nullptr);
              JSValueUnprotect(context->toJSContext(), resolve);
              JSValueUnprotect(context->toJSContext(), reject);
            });
          }
        } catch (const std::exception & e) {
          NX::Value message(ctx, e.what());
          JSValueRef args1[] { message.value(), nullptr };
          JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
          JSObjectCallAsFunction(ctx, reject, nullptr, 1, args2, exception);
          JSValueUnprotect(context->toJSContext(), resolve);
          JSValueUnprotect(context->toJSContext(), reject);
        }
        JSValueUnprotect(ctx, thisObject);
        return JSValueMakeUndefined(ctx);
      }), thisObject, 0, nullptr, nullptr);
  JSValueRef args[] { jsExecutor };
  JSValueRef exp = nullptr;
  JSObjectRef promise = JSObjectCallAsConstructor(ctx, Promise, 1, args, &exp);
  if (exp) {
    throw std::runtime_error(NX::Value(ctx, exp).toString());
  }
  return promise;
}

