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

#include <JavaScript.h>
#include <boost/function.hpp>

namespace NX {
  class Nexus;
  class AbstractTask;
  namespace Globals {
    class Promise
    {
      Promise() {}
      virtual ~Promise() {}
    public:

      static JSValueRef Get(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception);
      static constexpr JSStaticValue GetStaticProperty() {
        return JSStaticValue { "Promise", &NX::Globals::Promise::Get, nullptr, kJSPropertyAttributeNone };
      }

      typedef boost::function<void(JSValueRef)> ResolveRejectHandler;
      typedef boost::function<void(NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject)> Executor;

      static JSObjectRef createPromise(JSContextRef ctx, JSObjectRef executor, JSValueRef * exception);
      static JSObjectRef createPromise(JSContextRef ctx, const Executor & executor);
    };
  }
  using ResolveRejectHandler = Globals::Promise::ResolveRejectHandler;
}

#endif // GLOBALS_PROMISE_H
