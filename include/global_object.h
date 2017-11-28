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

#ifndef GLOBAL_OBJECT_H
#define GLOBAL_OBJECT_H

#ifdef BUILDING_WITH_CMAKE

#include <cmakeconfig.h>

#endif

#include <JavaScriptCore/runtime/JSCInlines.h>
#include <JavaScriptCore/API/JSCallbackObject.h>

namespace NX {
  class Nexus;

  using namespace JSC;

  class GlobalObject : public JSC::JSGlobalObject {
  protected:
    GlobalObject(JSC::VM &vm, JSC::Structure *structure);

    NX::Nexus *myNexus;
  public:
    typedef JSC::JSGlobalObject Base;

    static NX::GlobalObject *
    create(VM &vm, Structure *structure);

    NX::Nexus *nexus() const { return myNexus; }

    void setNexus(NX::Nexus *nx) { myNexus = nx; }

    static const bool needsDestruction = false;

  DECLARE_INFO

    static const GlobalObjectMethodTable s_NXGlobalObjectMethodTable;

    static Structure *createStructure(VM &vm, JSValue prototype) {
      auto result = Structure::create(vm, nullptr, prototype, TypeInfo(GlobalObjectType, StructureFlags), info());
      result->setTransitionWatchpointIsLikelyToBeFired(true);
      return result;
    }

    static RuntimeFlags javaScriptRuntimeFlags(const JSGlobalObject *) { return RuntimeFlags::createAllEnabled(); }

  protected:
    void finishCreation(VM &vm);

    void addFunction(VM &vm, JSObject *object, const char *name, NativeFunction function, unsigned arguments) {
      Identifier identifier = Identifier::fromString(&vm, name);
      object->putDirect(vm, identifier, JSFunction::create(vm, this, arguments, identifier.string(), function));
    }

    void addFunction(VM &vm, const char *name, NativeFunction function, unsigned arguments) {
      addFunction(vm, this, name, function, arguments);
    }

    void addConstructableFunction(VM &vm, const char *name, NativeFunction function, unsigned arguments) {
      Identifier identifier = Identifier::fromString(&vm, name);
      putDirect(vm, identifier,
                JSFunction::create(vm, this, arguments, identifier.string(), function, NoIntrinsic, function));
    }

    static JSInternalPromise *
    moduleLoaderImportModule(JSGlobalObject *, ExecState *, JSModuleLoader *, JSString *, JSValue,
                             const SourceOrigin &);

    static Identifier moduleLoaderResolve(JSGlobalObject *, ExecState *, JSModuleLoader *, JSValue, JSValue, JSValue);

    static JSInternalPromise *
    moduleLoaderFetch(JSGlobalObject *, ExecState *, JSModuleLoader *, JSValue, JSValue, JSValue);

    static JSObject *
    moduleLoaderCreateImportMetaProperties(JSGlobalObject *, ExecState *, JSModuleLoader *, JSValue, JSModuleRecord *,
                                           JSValue);

    static JSC::JSValue
    moduleLoaderEvaluate(JSC::JSGlobalObject *, JSC::ExecState *, JSC::JSModuleLoader *, JSC::JSValue, JSC::JSValue,
                         JSC::JSValue);

    static void queueTaskToEventLoop(JSC::JSGlobalObject &, Ref<JSC::Microtask> &&);

    static void promiseRejectionTracker(JSGlobalObject *jsGlobalObject, ExecState *exec, JSPromise *promise,
                                        JSPromiseRejectionOperation operation);

  };
}

#endif // GLOBAL_OBJECT_H
