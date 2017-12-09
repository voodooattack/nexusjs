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

#ifndef UTIL_H
#define UTIL_H

#include <JavaScriptCore/API/JSObjectRef.h>
#include <JavaScriptCore/API/JSValueRef.h>
#include <JavaScriptCore/API/JSContextRef.h>

#include <vector>

namespace NX
{

  JSObjectRef JSBindFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception);

  JSObjectRef JSCopyObjectShallow(JSContextRef source, JSContextRef dest, JSObjectRef object, JSValueRef * exception);

  JSValueRef JSWrapException(JSContextRef ctx, const std::exception & e, JSValueRef * exception);


  class ProtectedArguments: public std::vector<JSValueRef> {
  public:
    ProtectedArguments(JSContextRef ctx, size_t argumentCount, const JSValueRef arguments[]):
      std::vector<JSValueRef>(arguments, arguments + argumentCount), myContext(ctx)
    {
      for(auto i: *this)
        JSValueProtect(myContext, i);
    }
    ProtectedArguments(JSContextRef ctx, std::vector<JSValueRef> && values):
      std::vector<JSValueRef>(values), myContext(ctx)
    {
      for(auto i: *this)
        JSValueProtect(myContext, i);
    }
    ProtectedArguments(const ProtectedArguments & other): std::vector<JSValueRef>(other), myContext(other.myContext) {
      for(auto i: *this)
        JSValueProtect(myContext, i);
    }
    ProtectedArguments(ProtectedArguments && other) noexcept: std::vector<JSValueRef>(std::move(other)), myContext(other.myContext) {
      other.clear();
    }

    ~ProtectedArguments() {
      for(auto i: *this)
        JSValueUnprotect(myContext, i);
    }

    operator JSValueRef const *() const { return this->data(); }

  private:
    JSContextRef myContext;
  };

}
#endif // UTIL_H
