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
#include "global_object.h"
//#include "globals/global.h"
#include "scoped_string.h"

#include <JavaScriptCore/API/APICast.h>
#include <JavaScriptCore/parser/SourceCode.h>
#include <JavaScriptCore/parser/UnlinkedSourceCode.h>
#include <JavaScriptCore/runtime/Completion.h>
#include <JavaScriptCore/runtime/SourceOrigin.h>
#include <JavaScriptCore/runtime/JSCInlines.h>
#include <JavaScriptCore/runtime/JSInternalPromise.h>
#include <JavaScriptCore/runtime/FunctionPrototype.h>
#include <JavaScriptCore/runtime/JSFunction.h>
#include <JavaScriptCore/runtime/JSModuleLoader.h>

#include <JavaScriptCore/API/JSCallbackFunction.h>
#include <JavaScriptCore/API/JSCallbackObject.h>
#include <JavaScriptCore/runtime/InitializeThreading.h>
#include <JavaScriptCore/parser/ParserError.h>
#include <JavaScriptCore/heap/MachineStackMarker.h>

#include "globals/global.h"


JSValueRef NX::Context::evaluateScript(const std::string &src, JSObjectRef thisObject,
                                       const std::string &filePath, unsigned int lineNo, JSValueRef *exception) {
  NX::ScopedString srcRef(src);
  NX::ScopedString filePathRef(filePath);
  JSValueRef ret = JSEvaluateScript(toRef(myGlobal->globalExec()), srcRef, thisObject,
                                    filePath.length() ? (JSStringRef) filePathRef : nullptr, lineNo, exception);
  return ret;
}

JSC::JSInternalPromise * NX::Context::evaluateModule(const std::string &src, JSObjectRef thisObject,
                                       const std::string &filePath, unsigned int lineNo) {
  WTF::String code(src.c_str(), (unsigned) src.length());
  WTF::String path(filePath.c_str(), (unsigned) filePath.length());
  auto pos = WTF::TextPosition(WTF::OrdinalNumber::fromOneBasedInt(lineNo), OrdinalNumber());
  auto source = JSC::makeSource(code,
                                JSC::SourceOrigin { path },
                                path,
                                pos, JSC::SourceProviderSourceType::Module);
  auto exec = myGlobal->globalExec();
  JSLockHolder lockHolder(exec);
  JSC::ParserError e;
  auto valid = JSC::checkModuleSyntax(exec, source, e);
  if (valid) {
    return JSC::loadAndEvaluateModule(exec, source, JSC::jsUndefined());
  } else {
    NX::Nexus::ReportSyntaxError(source, e);
    return nullptr;
  }
}

NX::Context::~Context() {
  for(auto & val : myGlobals) {
    JSValueUnprotect(toRef(myGlobal->globalExec()), val.second);
  }
}

NX::Context::Context(NX::Context *parent, NX::Nexus *nx):
  myParent(parent), myNexus(nx), myVM(std::move(parent ? parent->myVM.copyRef() : JSC::VM::createContextGroup(LargeHeap))),
  myGlobal(nullptr), myGlobals()
{
  if (parent && !myNexus)
    myNexus = parent->myNexus;
  myNexus->scheduler()->scheduleThreadInitTask([=] { registerThread(); });
  myGlobal = createGlobalObject();
}

JSC::JSGlobalObject * NX::Context::createGlobalObject() {
  auto cls = myNexus->defineOrGetClass(Global::GlobalClass);
  JSC::JSLockHolder locker(myVM.ptr());
  auto cbStructure = JSC::JSCallbackObject<NX::GlobalObject>::createStructure(myVM, nullptr, JSC::jsNull());
  auto cbGlobalObject = JSC::JSCallbackObject<NX::GlobalObject>::create(myVM, cls, cbStructure);
  auto exec = cbGlobalObject->globalExec();
  JSValue prototype = cls->prototype(exec);
  if (!prototype)
    prototype = jsNull();
  cbGlobalObject->resetPrototype(myVM.get(), prototype);
  cbGlobalObject->setNexus(myNexus);
  cbGlobalObject->setPrivate(this);
  return cbGlobalObject;
}

NX::Context *NX::Context::FromJsContext(JSContextRef pContext) {
  auto exec = toJS(pContext);
  auto global = exec->lexicalGlobalObject();
  auto ptr = static_cast<JSCallbackObject<NX::GlobalObject>*>(global)->getPrivate();
  auto context = reinterpret_cast<NX::Context*>(ptr);
//  context->garbageCollect();
  return context;
}

JSGlobalContextRef NX::Context::toJSContext() const {
  if (myGlobal)
    return toGlobalRef(myGlobal->globalExec());
  return nullptr;
}

JSValueRef NX::Context::getGlobal(const char * name) {
  auto search = myGlobals.find(name);
  if (search == myGlobals.end()) {
    return nullptr;
  }
  return search->second;
}

JSValueRef NX::Context::getOrInitGlobal(JSContextRef ctx, const char *name) {
  auto search = myGlobals.find(name);
  if (search == myGlobals.end()) {
    auto exec = toJS(ctx);
    JSC::JSLockHolder lock(exec);
    auto ident = Identifier::fromString(exec, name);
    if (myGlobal->hasOwnProperty(exec, ident)) {
      auto value = myGlobal->get(exec, JSC::PropertyName(ident));
      if (!value.isUndefinedOrNull()) {
        auto global = toRef(myGlobal->globalExec(), value);
        return setGlobal(name, global);
      }
    }
    return nullptr;
  }
  return search->second;
}

JSValueRef NX::Context::setGlobal(const char * name, JSValueRef value) {
  JSValueProtect(toJSContext(), value);
  return myGlobals[name] = value;
}

JSObjectRef NX::Context::globalThisValue() const {
  return toRef(myGlobal->globalThis());
}

std::size_t NX::Context::garbageCollect() {
  JSC::VM& vm = myVM.get();;
  JSC::JSLockHolder lock(vm);
  vm.heap.collectNowFullIfNotDoneRecently(JSC::Sync);
  return vm.heap.sizeAfterLastFullCollection();
}

void NX::Context::registerThread() {
  if (myVM.ptr())
    myVM->heap.machineThreads().addCurrentThread();
}

