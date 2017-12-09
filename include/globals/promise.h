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


#ifndef GLOBALS_PROMISE_H
#define GLOBALS_PROMISE_H

#include <JavaScriptCore/API/JSContextRef.h>
#include <JavaScriptCore/API/JSObjectRef.h>
#include <JavaScriptCore/API/JSValueRef.h>

#include <boost/function.hpp>
#include <vector>

namespace NX {
  class Nexus;
  class AbstractTask;
  class Context;
  namespace Globals {
    class Promise
    {
      Promise() = default;
      virtual ~Promise() = default;
    public:

      static JSValueRef Get(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception);
      static constexpr JSStaticValue GetStaticProperty() {
        return JSStaticValue { "Promise", &NX::Globals::Promise::Get, nullptr, kJSPropertyAttributeNone };
      }

      typedef std::function<void(JSContextRef, JSValueRef)> ResolveRejectHandler;
      typedef std::function<void(JSContextRef, ResolveRejectHandler resolve, ResolveRejectHandler reject)> Executor;

      static JSValueRef createPromise(JSContextRef ctx, JSObjectRef executor, JSValueRef * exception);
      static JSObjectRef createPromise(JSContextRef ctx, const Executor & executor);
      static JSObjectRef all( JSContextRef ctx, const std::vector< JSValueRef > & promises );
      static JSObjectRef resolve( JSContextRef ctx, JSValueRef value );
      static JSObjectRef reject( JSContextRef ctx, JSValueRef value );
    };
  }
  using ResolveRejectHandler = Globals::Promise::ResolveRejectHandler;
}

#endif // GLOBALS_PROMISE_H
