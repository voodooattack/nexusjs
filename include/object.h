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
#include <boost/system/error_code.hpp>

#include "scoped_string.h"
#include "util.h"

namespace NX {
  class Value;
  class Object {
  public:
    Object(): myContext(nullptr), myObject(nullptr) {}
    Object(JSContextRef, JSClassRef cls = nullptr);
    Object(JSContextRef context, JSObjectRef obj);
    Object(JSContextRef context, JSValueRef val);
    Object(JSContextRef context, time_t val);
    Object(JSContextRef context, const std::exception & e);
    Object(JSContextRef context, const boost::system::error_code & e);
    Object(const Object & other);
    ~Object();

    std::string toString();

    std::shared_ptr<NX::Value> operator[] (unsigned int index);
    std::shared_ptr<NX::Value> operator[] (const char * name);

    std::shared_ptr<NX::Value> operator[] (unsigned int index) const;
    std::shared_ptr<NX::Value> operator[] (const char * name) const;

    void set(const std::string & name, JSValueRef value, JSPropertyAttributes attr = kJSPropertyAttributeNone,
             JSValueRef * exception = nullptr)
    {
      NX::ScopedString propertyName(name);
      JSObjectSetProperty(myContext, myObject, propertyName, value, attr, exception);
    }

    NX::Object & operator= (const NX::Object & other) {
      myContext = other.myContext;
      myObject = other.myObject;
      if (myContext && myObject)
        JSValueProtect(myContext, myObject);
      return *this;
    }

    JSContextRef context() const { return myContext; }
    JSObjectRef value() const { return myObject; }

    operator JSObjectRef() const { return myObject; }

    JSObjectRef construct(const std::vector<JSValueRef> & args = std::vector<JSValueRef>(), JSValueRef * exception = nullptr) {
      return JSObjectCallAsConstructor(myContext, myObject, args.size(), &args[0], exception);
    }

    JSValueRef call(JSObjectRef thisObject, const std::vector<JSValueRef> & args = std::vector<JSValueRef>(), JSValueRef * exception = nullptr) const {
      return JSObjectCallAsFunction(myContext, myObject, thisObject, args.size(), &args[0], exception);
    }

    JSValueRef call(JSObjectRef thisObject, const std::vector<JSValueRef> & args = std::vector<JSValueRef>(), JSValueRef * exception = nullptr) {
      return JSObjectCallAsFunction(myContext, myObject, thisObject, args.size(), &args[0], exception);
    }

    JSObjectRef bind(JSObjectRef thisObject, size_t argumentCount,
                     const JSValueRef arguments[], JSValueRef * exception)
    {
      return JSBindFunction(myContext, myObject, thisObject, argumentCount, arguments, exception);
    }

    void * getPrivate() { return JSObjectGetPrivate(myObject); }
    void setPrivate(void * priv) { JSObjectSetPrivate(myObject, priv); }

    typedef std::function<JSValueRef(JSContextRef, JSValueRef, JSValueRef *)> PromiseCallback;

    NX::Object then(PromiseCallback onResolve, PromiseCallback onReject = PromiseCallback());

  private:
    JSContextRef myContext;
    JSObjectRef myObject;
  };
}

#endif // OBJECT_H
