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
  NX::Module * module = Module::FromContext(ctx);
  if (JSObjectRef Promise = module->getGlobal("Promise"))
    return Promise;
  JSValueRef Promise = module->evaluateScript(std::string(promise_js, promise_js + promise_js_len),
                                              nullptr, "promise.js", 1, exception);
  JSObjectRef promiseObject = JSValueToObject(module->context(), Promise, exception);
  if (!*exception)
    return module->setGlobal("Promise", promiseObject);
  return JSValueMakeUndefined(ctx);
}

JSObjectRef NX::Globals::Promise::createPromise (JSContextRef ctx, JSObjectRef executor, JSValueRef * exception)
{
  NX::Module * module = Module::FromContext(ctx);
  JSObjectRef Promise = module->getOrInitGlobal("Promise");
  JSValueRef args[] { executor };
  return JSObjectCallAsConstructor(ctx, Promise, 1, args, exception);
}
