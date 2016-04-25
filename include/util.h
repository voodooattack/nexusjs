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

#include <JavaScript.h>
#include <vector>

namespace NX
{

  JSObjectRef JSBindFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception);

  JSObjectRef JSCopyObjectShallow(JSContextRef source, JSContextRef dest, JSObjectRef object, JSValueRef * exception);

  JSValueRef JSWrapException(JSContextRef ctx, const std::exception & e, JSValueRef * exception);


  class ProtectedArguments {
  public:
    ProtectedArguments(JSContextRef ctx, size_t argumentCount, const JSValueRef arguments[]):
    myContext(ctx), myArguments(arguments, arguments + argumentCount)
    {
      for(auto i: myArguments)
        JSValueProtect(myContext, i);
    }
    ProtectedArguments(const ProtectedArguments & other): myContext(other.myContext), myArguments(other.myArguments) {
      for(auto i: myArguments)
        JSValueProtect(myContext, i);
    }
    ~ProtectedArguments() {
      for(auto i: myArguments)
        JSValueUnprotect(myContext, i);
    }

    JSValueRef operator[](unsigned int index) const { return myArguments[index]; }

  private:
    JSContextRef myContext;
    std::vector<JSValueRef> myArguments;
  };
  
}
#endif // UTIL_H
