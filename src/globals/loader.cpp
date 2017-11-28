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
#include "context.h"
#include "globals/loader.h"

#include "loader.js.inc"

JSValueRef NX::Globals::Loader::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  if (auto Loader = context->getGlobal("Loader"))
      return Loader;
  JSValueRef Loader = context->evaluateScript(std::string(loader_js, loader_js + loader_js_len),
                                              nullptr, "loader.js", 1, exception);
  JSObjectRef loaderObject = JSValueToObject(context->toJSContext(), Loader, exception);
  if (!*exception)
    return context->setGlobal("Loader", loaderObject);
  return JSValueMakeUndefined(ctx);
}

