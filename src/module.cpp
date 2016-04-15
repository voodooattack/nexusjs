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

#include "module.h"
#include "object.h"
#include "value.h"
#include "globals/global.h"

NX::Module::Module (NX::Nexus * nx, JSContextGroupRef group, JSClassRef globalClass) :
  myNexus(nx), myGroup (group), myContext (nullptr), myGlobalObject (nullptr), myModuleObject(nullptr), myGlobals()
{
  JSClassRef gClass = globalClass ? globalClass : JSClassCreate(&Global::GlobalClass);
  myContext = JSGlobalContextCreateInGroup(group, gClass);
  myGlobalObject = JSContextGetGlobalObject(myContext);
  JSObjectSetPrivate(myGlobalObject, this);
  if (!globalClass)
    JSClassRelease(gClass);
  myModuleObject = getObject(nullptr);
  NX::Object(myContext, myGlobalObject).set("module", myModuleObject);
}

JSValueRef NX::Module::evaluateScript (const std::string & src, JSObjectRef thisObject,
                                       const std::string & filePath, unsigned int lineNo, JSValueRef * exception)
{
  JSStringRef srcRef = JSStringCreateWithUTF8CString(src.c_str());
  JSStringRef filePathRef = filePath.length() ? JSStringCreateWithUTF8CString(filePath.c_str()) : nullptr;
  JSValueRef ret = JSEvaluateScript(myContext, srcRef, thisObject, filePathRef, lineNo, exception);
  myGenericClass = JSClassCreate(&kJSClassDefinitionEmpty);
  return ret;
}

NX::Module::~Module()
{
  JSGlobalContextRelease(myContext);
  for(auto & c : myObjectClasses)
    JSClassRelease(c.second);
  JSClassRelease(myGenericClass);
}

JSObjectRef NX::Module::getObject(JSValueRef * exception)
{
  if (myModuleObject)
    return myModuleObject;
  JSObjectRef moduleObject = NX::Object(myContext, myGlobalObject)["Module"]->toObject()->construct(std::vector<JSValueRef>(), exception);
  return myModuleObject = moduleObject;
}



