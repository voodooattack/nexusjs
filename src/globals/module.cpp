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
#include "globals/module.h"

#include "module.js.inc"

static const std::vector<JSChar> moduleJS(module_js, module_js + module_js_len);

JSValueRef NX::Globals::Module::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Nexus * nx = reinterpret_cast<NX::Nexus*>(JSObjectGetPrivate(object));
  if (nx->globals().find("Module") != nx->globals().end())
    return nx->globals()["Module"];
  JSStringRef script = JSStringCreateWithCharacters(moduleJS.data(), moduleJS.size());
  JSStringRef scriptName = JSStringCreateWithUTF8CString("module.js");
  JSValueRef Module = JSEvaluateScript(ctx, script, object, scriptName, 1, exception);
  JSStringRelease(script);
  JSStringRelease(scriptName);
  if (!*exception)
    nx->globals()["Module"] = JSValueToObject(ctx, Module, exception);
  return Module;
}
