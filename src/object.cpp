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

#include "object.h"
#include "value.h"

#include <stdexcept>

NX::Object::Object (JSContextRef context, JSClassRef cls): myContext(context), myObject(nullptr)
{
  JSValueRef exception = nullptr;
  myObject = JSObjectMake(context, cls, &exception);
  if (exception) {
    NX::Value except(myContext, exception);
    throw std::runtime_error(except.toString());
  }
  JSValueProtect(context, myObject);
}

NX::Object::Object (JSContextRef context, JSObjectRef obj): myContext(context), myObject(obj)
{
  JSValueProtect(context, obj);
}

NX::Object::Object (JSContextRef context, JSValueRef val): myContext(context), myObject(nullptr)
{
  JSValueRef exception = nullptr;
  myObject = JSValueToObject(myContext, val, &exception);
  if (exception) {
    NX::Value except(myContext, exception);
    throw std::runtime_error(except.toString());
  }
  JSValueProtect(context, myObject);
}


NX::Object::~Object()
{
  JSValueUnprotect(myContext, myObject);
}

boost::shared_ptr<NX::Value> NX::Object::operator[] (const char * name)
{
  JSStringRef nameRef = JSStringCreateWithUTF8CString(name);
  JSValueRef val = JSObjectGetProperty(myContext, myObject, nameRef, nullptr);
  JSStringRelease(nameRef);
  return boost::shared_ptr<NX::Value>(new NX::Value(myContext, val));
}

boost::shared_ptr<NX::Value> NX::Object::operator[] (unsigned int index)
{
  JSValueRef val = JSObjectGetPropertyAtIndex(myContext, myObject, index, nullptr);
  return boost::shared_ptr<NX::Value>(new NX::Value(myContext, val));
}

std::string NX::Object::toString()
{
  NX::Value val(myContext, myObject);
  return val.toString();
}
