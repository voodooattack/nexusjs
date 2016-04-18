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
#include "object.h"
#include "value.h"
#include "globals/global.h"

NX::Context::Context (NX::Context * parent, NX::Nexus * nx, JSContextGroupRef group, JSClassRef globalClass) :
  myNexus(parent ? parent->nexus() : nx), myGroup (parent ? parent->group() : group),
  myContext (nullptr), myGlobals(), myGlobalObject (nullptr), myModuleObject(nullptr),
  myGenericClass(), myObjectClasses(), myParent(parent), myChildren()
{
  if (myParent)
    myParent->myChildren.push_back(std::shared_ptr<NX::Context>(this));
  JSClassRef gClass = globalClass ? globalClass : JSClassCreate(&Global::GlobalClass);
  myContext = group ? JSGlobalContextCreateInGroup(group, gClass) : JSGlobalContextCreate(gClass);
  myGlobalObject = JSContextGetGlobalObject(myContext);
  if (!globalClass)
    JSClassRelease(gClass);
  setGlobal("module", myModuleObject);
  JSValueRef exception = nullptr;
  initGlobal(myGlobalObject, &exception);
  if (exception)
    nx->ReportException(myContext, exception);
}

JSValueRef NX::Context::evaluateScript (const std::string & src, JSObjectRef thisObject,
                                       const std::string & filePath, unsigned int lineNo, JSValueRef * exception)
{
  JSStringRef srcRef = JSStringCreateWithUTF8CString(src.c_str());
  JSStringRef filePathRef = filePath.length() ? JSStringCreateWithUTF8CString(filePath.c_str()) : nullptr;
  JSValueRef ret = JSEvaluateScript(myContext, srcRef, thisObject, filePathRef, lineNo, exception);
  myGenericClass = JSClassCreate(&kJSClassDefinitionEmpty);
  return ret;
}

NX::Context::~Context()
{
  for(auto & o : myGlobals)
    JSValueUnprotect(myContext, o.second);
  for(auto & c : myObjectClasses)
    JSClassRelease(c.second);
  JSClassRelease(myGenericClass);
  JSGlobalContextRelease(myContext);
}

JSObjectRef NX::Context::getModuleObject(JSValueRef * exception)
{
  if (myModuleObject)
    return myModuleObject;
  JSObjectRef moduleObject = NX::Object(myContext, myGlobalObject)["Module"]->toObject()->construct(std::vector<JSValueRef>(), exception);
  return myModuleObject = moduleObject;
}

void NX::Context::initGlobal (JSObjectRef object, JSValueRef * exception)
{
  JSObjectSetPrivate(object, this);
  NX::Object global(myContext, object);
  global.set("module", getModuleObject(exception));
  if (exception && *exception) return;
  JSObjectRef systemObject = global["Loader"]->toObject()->construct(std::vector<JSValueRef>(), exception);
  if (exception && *exception) return;
  global.set("System", systemObject, kJSPropertyAttributeNone, exception);
  if (exception && *exception) return;
  setGlobal("System", systemObject);
}


