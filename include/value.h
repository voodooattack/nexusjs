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

#ifndef VALUE_H
#define VALUE_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <JavaScript.h>

namespace NX {
  class Object;
  class Value: public boost::noncopyable {
  public:
    Value(JSContextRef context, JSValueRef val);
    Value(JSContextRef context, JSStringRef strRef);
    Value(JSContextRef context, const std::string & str);
    Value(JSContextRef context, double value);
    ~Value();

    boost::shared_ptr<NX::Value> operator[] (unsigned int index);

    boost::shared_ptr<NX::Object> toObject();
    bool toBoolean();
    double toNumber();
    std::string toString();
    std::string toJSON(unsigned int indent = 0);

    bool isObject() { return JSValueIsObject(myContext, myVal); }
    bool isFunction() { return isObject() && JSObjectIsFunction(myContext, JSValueToObject(myContext, myVal, nullptr)); }
    JSValueRef value() { return myVal; }

  private:
    JSContextRef myContext;
    JSValueRef myVal;
  };
}

#endif // VALUE_H
