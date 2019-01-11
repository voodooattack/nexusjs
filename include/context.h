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

#include <JavaScriptCore/runtime/JSInternalPromise.h>
#include <JavaScriptCore/API/JSCallbackObject.h>

#include <string>
#include <boost/unordered_map.hpp>

#include "object.h"
#include "value.h"
#include "scoped_string.h"

namespace NX {
  class Nexus;
  class GlobalObject;
  class Context
  {
  protected:
    explicit Context(NX::Nexus * nx, WTF::Ref<JSC::VM> vm);
  public:
    virtual ~Context();

    JSValueRef evaluateScript(const std::string & src,
                              JSObjectRef thisObject = nullptr,
                              const std::string & filePath = "",
                              unsigned int lineNo = 1,
                              JSValueRef * exception = nullptr);
    JSC::JSInternalPromise * evaluateModule(const std::string & src,
                              JSObjectRef thisObject = nullptr,
                              const std::string & filePath = "",
                              unsigned int lineNo = 1);

    NX::Nexus * nexus() { return myNexus; }

    JSGlobalContextRef toJSContext() const;
    static Context *FromJsContext(JSContextRef pContext);

    JSValueRef getGlobal(const char * name);
    JSValueRef getOrInitGlobal(JSContextRef ctx, const char * name);
    JSValueRef setGlobal(const char * name, JSValueRef value);

    JSObjectRef globalThisValue() const;
    JSC::JSGlobalObject * globalObject() const { return myGlobal; }

    JSC::VM * vm() const { return myVM.ptr(); }

    void registerThread();

    std::size_t garbageCollect();

    static NX::Context * create(NX::Nexus * nx);

    virtual NX::Context * clone();

  protected:

    JSC::JSGlobalObject * createGlobalObject();

    NX::Context * myParent;
    NX::Nexus * myNexus;
    WTF::Ref<JSC::VM> myVM;
    JSC::JSGlobalObject * myGlobal;
    boost::unordered_map<std::string, JSValueRef> myGlobals;
  };
}

#endif // MODULE_H
