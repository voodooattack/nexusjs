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

#include <JavaScript.h>
#include <string>
#include <boost/unordered_map.hpp>

#include "object.h"
#include "value.h"
#include "scoped_string.h"

namespace NX {
  class Nexus;
  class Context
  {
  public:
    Context(NX::Context * parent = nullptr, NX::Nexus * nx = nullptr, JSContextGroupRef group = nullptr, JSClassRef globalClass = nullptr);
    virtual ~Context();

    JSValueRef evaluateScript(const std::string & src,
                              JSObjectRef thisObject = nullptr,
                              const std::string & filePath = "",
                              unsigned int lineNo = 1,
                              JSValueRef * exception = nullptr);

    bool isDetached() { return myGroup != nullptr; }

    boost::unordered_map<std::string, JSObjectRef> & globals() { return myGlobals; }

    JSGlobalContextRef toJSContext() { return myContext; }
    NX::Nexus * nexus() { return myNexus; }

    JSObjectRef getModuleObject(JSValueRef * exception);
    JSContextGroupRef group() { return myGroup; }

    JSObjectRef getGlobal(const std::string & name) {
      return myGlobals[name];
    }

    JSObjectRef globalObject() { return myGlobalObject; }

    JSObjectRef getOrInitGlobal(const std::string & name) {
      if (!myGlobals[name]) {
        NX::ScopedString propName(name);
        JSValueRef value = JSObjectGetProperty(myContext, JSContextGetGlobalObject(myContext), propName, nullptr);
        if (!JSValueIsUndefined(myContext, value)) {
          JSObjectRef object = JSValueToObject(myContext, value, nullptr);
          if (object)
            return myGlobals[name] = object;
        }
        return nullptr;
      }
      return myGlobals[name];
    }

    JSValueRef exports() {
      return NX::Object(myContext, myModuleObject)["exports"]->value();
    }

    JSObjectRef setGlobal(const std::string & name, JSObjectRef object) {
      JSValueProtect(myContext, object);
      return myGlobals[name] = object;
    }

    void initGlobal(JSObjectRef object, JSValueRef * exception);

    static Context * FromJsContext(JSContextRef ctx) {
      return reinterpret_cast<NX::Context*>(JSObjectGetPrivate(
        JSContextGetGlobalObject(JSContextGetGlobalContext(ctx))));
    }

  protected:
    NX::Nexus * myNexus;
    JSContextGroupRef myGroup;
    JSGlobalContextRef myContext;
    boost::unordered_map<std::string, JSObjectRef> myGlobals;
    JSObjectRef myGlobalObject, myModuleObject;
    NX::Context * myParent;
  };
}

#endif // MODULE_H
