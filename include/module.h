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

#ifndef NEXUS_MODULE_H
#define NEXUS_MODULE_H

#include <JavaScriptCore/API/JSBase.h>
#include <JavaScriptCore/API/APICast.h>
#include <JavaScriptCore/heap/Heap.h>
#include <JavaScriptCore/heap/HeapInlines.h>
#include <JavaScriptCore/heap/HeapCell.h>
#include <JavaScriptCore/heap/HeapCellInlines.h>
#include <JavaScriptCore/runtime/VM.h>
#include <JavaScriptCore/runtime/JSModuleLoader.h>
#include <JavaScriptCore/runtime/Completion.h>
#include <JavaScriptCore/runtime/Symbol.h>
#include <JavaScriptCore/runtime/CatchScope.h>
#include <JavaScriptCore/runtime/JSInternalPromiseDeferred.h>
#include <JavaScriptCore/runtime/JSCJSValue.h>
#include <JavaScriptCore/runtime/IdentifierInlines.h>

JSC::JSInternalPromise* rejectPromise(JSC::ExecState* exec, JSC::JSGlobalObject* globalObject)
{
  JSC::VM& vm = exec->vm();
  auto scope = DECLARE_CATCH_SCOPE(vm);
  scope.assertNoException();
  JSC::JSValue exception = scope.exception()->value();
  scope.clearException();
  JSC::JSInternalPromiseDeferred* deferred = JSC::JSInternalPromiseDeferred::create(exec, globalObject);
  deferred->reject(exec, exception);
  return deferred->promise();
}


JSC::Symbol* createSymbolForEntryPointModule(JSC::VM& vm)
{
  // Generate the unique key for the source-provided module.
  JSC::PrivateName privateName(JSC::PrivateName::Description, "EntryPointModule");
  return JSC::Symbol::create(vm, privateName.uid());
}

JSC::JSInternalPromise* loadAndEvaluateModule(JSC::ExecState* exec, const WTF::String& moduleName, JSC::JSValue parameters, JSC::JSValue scriptFetcher)
{
  JSC::VM& vm = exec->vm();
  JSC::JSLockHolder lock(vm);

  return exec->vmEntryGlobalObject()->moduleLoader()
      ->loadAndEvaluateModule(exec, identifierToJSValue(vm, JSC::Identifier::fromString(exec, moduleName)),
                              parameters,
                              scriptFetcher);
}

JSC::JSInternalPromise* loadAndEvaluateModule(JSC::ExecState* exec, const JSC::SourceCode& source, JSC::JSValue scriptFetcher)
{
  JSC::VM& vm = exec->vm();
  JSC::JSLockHolder lock(vm);
  auto scope = DECLARE_THROW_SCOPE(vm);

  JSC::Symbol* key = createSymbolForEntryPointModule(vm);

  JSC::JSGlobalObject* globalObject = exec->vmEntryGlobalObject();

  // Insert the given source code to the ModuleLoader registry as the fetched registry entry.
  globalObject->moduleLoader()->provideFetch(exec, key, source);
  RETURN_IF_EXCEPTION(scope, rejectPromise(exec, globalObject));

  return globalObject->moduleLoader()->loadAndEvaluateModule(exec, key, JSC::jsUndefined(), scriptFetcher);
}

JSC::JSInternalPromise* loadModule(JSC::ExecState* exec, const WTF::String& moduleName, JSC::JSValue parameters, JSC::JSValue scriptFetcher)
{
  JSC::VM& vm = exec->vm();
  JSC::JSLockHolder lock(vm);
  return exec->vmEntryGlobalObject()->moduleLoader()
      ->loadModule(exec, identifierToJSValue(vm, JSC::Identifier::fromString(exec, moduleName)), parameters, scriptFetcher);
}

#endif //NEXUS_MODULE_H
