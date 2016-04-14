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

#include "globals/promise.h"
#include "nexus.h"

#include "promise.js.inc"

static const std::vector<JSChar> promiseJS(promise_js, promise_js + promise_js_len);

JSValueRef NX::Globals::Promise::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Nexus * nx = reinterpret_cast<NX::Nexus*>(JSObjectGetPrivate(object));
  if (nx->globals().find("Promise") != nx->globals().end())
    return nx->globals()["Promise"];
  JSStringRef script = JSStringCreateWithCharacters(promiseJS.data(), promiseJS.size());
  JSStringRef scriptName = JSStringCreateWithUTF8CString("promise.js");
  JSValueRef Promise = JSEvaluateScript(ctx, script, object, scriptName, 1, exception);
  JSStringRelease(script);
  JSStringRelease(scriptName);
  if (!*exception)
    nx->globals()["Promise"] = JSValueToObject(ctx, Promise, exception);
  return Promise;
}

JSObjectRef NX::Globals::Promise::createPromise (JSContextRef ctx, JSObjectRef executor, JSValueRef * exception)
{
  NX::Nexus * nx = reinterpret_cast<NX::Nexus*>(JSObjectGetPrivate(JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
  JSObjectRef Promise = nx->globals()["Promise"];
  JSValueRef args[] { executor };
  return JSObjectCallAsConstructor(ctx, Promise, 1, args, exception);
}
