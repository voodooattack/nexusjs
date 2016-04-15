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

#ifndef MODULE_H
#define MODULE_H

#include <JavaScriptCore.h>
#include <string>
#include <boost/unordered_map.hpp>

namespace NX {
  class Nexus;
  class Module
  {
  public:
    Module(NX::Nexus * nx, JSContextGroupRef group, JSClassRef globalClass = nullptr);
    virtual ~Module();

    JSValueRef evaluateScript(const std::string & src,
                              JSObjectRef thisObject = nullptr,
                              const std::string & filePath = "",
                              unsigned int lineNo = 1,
                              JSValueRef * exception = nullptr);

    JSClassRef defineOrGetClass(const JSClassDefinition & def) {
      if (myObjectClasses.find(def.className) != myObjectClasses.end())
        return myObjectClasses[def.className];
      return myObjectClasses[def.className] = JSClassCreate(&def);
    }

    boost::unordered_map<std::string, JSObjectRef> & globals() { return myGlobals; }

    JSGlobalContextRef context() { return myContext; }
    NX::Nexus * nexus() { return myNexus; }

    JSObjectRef getObject(JSValueRef * exception);

  protected:
    NX::Nexus * myNexus;
    JSContextGroupRef myGroup;
    JSGlobalContextRef myContext;
    JSObjectRef myGlobalObject, myModuleObject;
    JSClassRef myGenericClass;
    boost::unordered_map<std::string, JSObjectRef> myGlobals;
    boost::unordered_map<std::string, JSClassRef> myObjectClasses;
  };
}

#endif // MODULE_H
