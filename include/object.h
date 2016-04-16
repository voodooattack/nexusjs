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


#ifndef OBJECT_H
#define OBJECT_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <JavaScript.h>

namespace NX {
  class Value;
  class Object {
  public:
    Object(JSContextRef, JSClassRef cls = nullptr);
    Object(JSContextRef context, JSObjectRef obj);
    Object(JSContextRef context, JSValueRef val);
    ~Object();

    std::string toString();

    boost::shared_ptr<NX::Value> operator[] (unsigned int index);
    boost::shared_ptr<NX::Value> operator[] (const char * name);

    void set(const std::string & name, JSValueRef value, JSPropertyAttributes attr = kJSPropertyAttributeNone,
             JSValueRef * exception = nullptr) {
      JSStringRef propertyName = JSStringCreateWithUTF8CString(name.c_str());
      JSObjectSetProperty(myContext, myObject, propertyName, value, attr, exception);
      JSStringRelease(propertyName);
    }

    JSObjectRef value() { return myObject; }

    JSObjectRef construct(const std::vector<JSValueRef> & args = std::vector<JSValueRef>(), JSValueRef * exception = nullptr) {
      return JSObjectCallAsConstructor(myContext, myObject, args.size(), &args[0], exception);
    }

  private:
    JSContextRef myContext;
    JSObjectRef myObject;
  };
}

#endif // OBJECT_H
