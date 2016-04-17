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

namespace NX {
  JSObjectRef JSBindFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
  {
    JSStringRef strBind = JSStringCreateWithUTF8CString("Function.__proto__.bind");
    JSValueRef bind = JSEvaluateScript(ctx, strBind, JSContextGetGlobalObject(ctx), nullptr, 0, exception);
    JSStringRelease(strBind);
    std::vector<JSValueRef> args;
    args.push_back(thisObject);
    for(int i = 0; i < argumentCount; i++)
        args.push_back(arguments[i]);
    return JSValueToObject(ctx,
      JSObjectCallAsFunction(ctx, JSValueToObject(ctx, bind, exception), function, args.size(), &args[0], exception), exception);
  }
}

#endif // UTIL_H
