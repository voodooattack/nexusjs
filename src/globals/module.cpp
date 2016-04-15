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
#include "value.h"
#include "globals/module.h"

#include "module.js.inc"

JSValueRef NX::Globals::Module::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Module * module = reinterpret_cast<NX::Module*>(JSObjectGetPrivate(JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
  if (JSObjectRef Module = module->getGlobal("Module"))
    return Module;
  JSValueRef Module = module->evaluateScript(std::string(module_js, module_js + module_js_len),
                                             nullptr, "module.js", 1, exception);
  JSObjectRef moduleObject = JSValueToObject(module->context(), Module, exception);
  if (!*exception)
    return module->setGlobal("Module", moduleObject);
  return JSValueMakeUndefined(ctx);
}

