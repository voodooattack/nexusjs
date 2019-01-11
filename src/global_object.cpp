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
#include "global_object.h"

#include <wtf/Optional.h>

#include <wtf/text/StringBuilder.h>

#include <JavaScriptCore/runtime/JSGenericTypedArrayView.h>
#include <JavaScriptCore/runtime/TypedArrayAdaptors.h>
#include <JavaScriptCore/runtime/JSInternalPromiseDeferred.h>
#include <JavaScriptCore/runtime/CatchScope.h>
#include <JavaScriptCore/runtime/Completion.h>
#include <JavaScriptCore/runtime/JSSourceCode.h>

#include <JavaScriptCore/runtime/Uint8Array.h>
#include <JavaScriptCore/runtime/ObjectConstructor.h>
#include <JavaScriptCore/runtime/JSModuleLoader.h>
#include <JavaScriptCore/runtime/GenericTypedArrayViewInlines.h>
#include <JavaScriptCore/runtime/JSModuleRecord.h>

#include <JavaScriptCore/parser/ParserError.h>
#include <JavaScriptCore/builtins/BuiltinUtils.h>

#include "globals/global.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <curl/curl.h>

namespace JSC {
  template<>
  Structure *JSCallbackObject<NX::GlobalObject>::createStructure(VM &vm, JSGlobalObject *globalObject, JSValue proto) {
    return Structure::create(vm, globalObject, proto, TypeInfo(GlobalObjectType, StructureFlags), info());
  }

  template<>
  JSCallbackObject<NX::GlobalObject> *
  JSCallbackObject<NX::GlobalObject>::create(VM &vm, JSClassRef classRef, Structure *structure) {
    auto callbackObject = new(NotNull, allocateCell<JSCallbackObject<NX::GlobalObject>>(vm.heap))
        JSCallbackObject(vm, classRef, structure);
    callbackObject->finishCreation(vm);
    return callbackObject;
  }
}

using namespace JSC;

const ClassInfo NX::GlobalObject::s_info = {
    "NXGlobalObject", Base::info(), nullptr, nullptr,
    CREATE_METHOD_TABLE(NX::GlobalObject)
};

template<>
const ClassInfo JSC::JSCallbackObject<NX::GlobalObject>::s_info = {
    "NXCallbackGlobalObject", Base::info(), nullptr, nullptr,
    CREATE_METHOD_TABLE(JSC::JSCallbackObject<NX::GlobalObject>)
};

const GlobalObjectMethodTable NX::GlobalObject::s_NXGlobalObjectMethodTable = {
    &NX::GlobalObject::supportsRichSourceInfo,
    &NX::GlobalObject::shouldInterruptScript,
    &NX::GlobalObject::javaScriptRuntimeFlags,
    &NX::GlobalObject::queueTaskToEventLoop,
    &NX::GlobalObject::shouldInterruptScriptBeforeTimeout,
    &NX::GlobalObject::moduleLoaderImportModule,
    &NX::GlobalObject::moduleLoaderResolve,
    &NX::GlobalObject::moduleLoaderFetch,
    &NX::GlobalObject::moduleLoaderCreateImportMetaProperties,
    &NX::GlobalObject::moduleLoaderEvaluate,
    &NX::GlobalObject::promiseRejectionTracker,
    nullptr, // defaultLanguage
};

void NX::GlobalObject::queueTaskToEventLoop(JSC::JSGlobalObject &global, Ref<JSC::Microtask> &&task) {
  auto & globalObject = reinterpret_cast<JSCallbackObject<NX::GlobalObject>&>(global);
  auto &t = task.leakRef();
  globalObject.nexus()->scheduler()->scheduleTask(
    [&]() {
      auto exec = globalObject.globalExec();
      JSC::JSLockHolder lock(exec);
      t.run(exec);
      t.deref();
    }
  );
}

NX::GlobalObject::GlobalObject(JSC::VM &vm, JSC::Structure *structure) :
    JSGlobalObject(vm, structure, &NX::GlobalObject::s_NXGlobalObjectMethodTable), myNexus(nullptr) {}

void NX::GlobalObject::finishCreation(VM &vm) {
  Base::finishCreation(vm);
//  WebCore::JSBuiltinInternalFunctions::initialize(*this);
}

NX::GlobalObject *NX::GlobalObject::create(VM &vm, Structure *structure) {
  auto object = new(NotNull, allocateCell<NX::GlobalObject>(vm.heap)) NX::GlobalObject(vm, structure);
  object->finishCreation(vm);
  return object;
}

struct DirectoryName {
  // In unix, it is "/". In Windows, it becomes a drive letter like "C:\"
  String rootName;

  // If the directory name is "/home/WebKit", this becomes "home/WebKit". If the directory name is "/", this becomes "".
  String queryName;
};

struct ModuleName {
  explicit ModuleName(const String &moduleName) {
    queries = moduleName.splitAllowingEmptyEntries('/');
  }

  bool startsWithRoot() const {
    return !queries.isEmpty() && queries[0].isEmpty();
  }

  WTF::Vector<WTF::String> queries;
};

static UChar pathSeparator() {
  return boost::filesystem::path::preferred_separator;
}

static WTF::Optional<DirectoryName> extractDirectoryName(const String &absolutePathToFile) {
  size_t firstSeparatorPosition = absolutePathToFile.find(pathSeparator());
  if (firstSeparatorPosition == notFound)
    return WTF::nullopt;
  DirectoryName directoryName;
  directoryName.rootName = absolutePathToFile.substring(0, (unsigned) firstSeparatorPosition +
                                                           1); // Include the separator.
  size_t lastSeparatorPosition = absolutePathToFile.reverseFind(pathSeparator());
  ASSERT_WITH_MESSAGE(lastSeparatorPosition != notFound,
                      "If the separator is not found, this function already returns when performing the forward search.");
  if (firstSeparatorPosition == lastSeparatorPosition)
    directoryName.queryName = StringImpl::empty();
  else {
    size_t queryStartPosition = firstSeparatorPosition + 1;
    size_t queryLength = lastSeparatorPosition - queryStartPosition; // Not include the last separator.
    directoryName.queryName = absolutePathToFile.substring((unsigned) queryStartPosition, (unsigned) queryLength);
  }
  return directoryName;
}

static WTF::String resolvePath(const DirectoryName &directoryName, const ModuleName &moduleName) {

  WTF::Vector<WTF::String> directoryPieces(directoryName.queryName.splitAllowingEmptyEntries(pathSeparator()));

  // Only first '/' is recognized as the path from the root.
  if (moduleName.startsWithRoot())
    directoryPieces.clear();

  for (const auto &query : moduleName.queries) {
    if (query == String("..")) {
      if (!directoryPieces.isEmpty())
        directoryPieces.removeLast();
    } else if (!query.isEmpty() && query != String("."))
      directoryPieces.append(query);
  }

  StringBuilder builder;
  builder.append(directoryName.rootName);
  for (size_t i = 0; i < directoryPieces.size(); ++i) {
    builder.append(directoryPieces[i]);
    if (i + 1 != directoryPieces.size())
      builder.append(pathSeparator());
  }
  return builder.toString();
}

static WTF::Optional<DirectoryName> currentWorkingDirectory() {
  String directoryString = String::fromUTF8(boost::filesystem::current_path().c_str());
  if (directoryString.isEmpty())
    return WTF::nullopt;
  if (directoryString[directoryString.length() - 1] == pathSeparator())
    return extractDirectoryName(directoryString);
  // Append the separator to represent the file name. extractDirectoryName only accepts the absolute file name.
  return extractDirectoryName(makeString(directoryString, pathSeparator()));
}

static void convertShebangToJSComment(WTF::Vector<uint8_t> &buffer) {
  if (buffer.size() >= 2) {
    if (buffer[0] == '#' && buffer[1] == '!')
      buffer[0] = buffer[1] = '/';
  }
}

template<typename Vector>
static inline WTF::String stringFromUTF(const Vector &utf8) {
  return WTF::String::fromUTF8WithLatin1Fallback(utf8.data(), utf8.size());
}

static bool fetchModuleFromLocalFileSystem(const WTF::String &fileName, WTF::Vector<uint8_t> &buffer) {
  try {
    const boost::filesystem::path path(fileName.utf8().buffer()->data());
    if (!boost::filesystem::exists(path)) {
      static const std::list<const char *> extensions { ".mjs", ".js" };
      for(auto const & ext : extensions) {
        boost::filesystem::path withExt(path);
        withExt.concat(ext);
        if (boost::filesystem::exists(withExt)) {
          WTF::String fileNameWithExt(withExt.c_str());
          return fetchModuleFromLocalFileSystem(fileNameWithExt, buffer);
        }
      }
      return false;
    } else if (boost::filesystem::is_directory(path)) {
      boost::filesystem::path p(path);
      p.append("index");
      WTF::String fileNameWithIndexJs(p.c_str());
      return fetchModuleFromLocalFileSystem(fileNameWithIndexJs, buffer);
    }
    boost::filesystem::ifstream stream(path, std::ifstream::in | std::ifstream::binary);
    stream.unsetf(std::ios_base::skipws);
    if (stream.bad())
      return false;
    buffer.appendRange(std::istream_iterator<char>(stream), std::istream_iterator<char>());
    convertShebangToJSComment(buffer);
    return true;
  } catch (const std::exception &e) {
    NX::Nexus::ReportException(e);
    return false;
  }
}

#define URL_REGEX "([a-zA-Z0-9]+://)?([^/ :]+)?:?([^/ ]*)(/?[^ #?]*)?\\x3f?([^ #]*)?#?([^ ]*)?"

static bool isURL(const WTF::String &url) {
  boost::regex ex(URL_REGEX);
  boost::cmatch what;
  return regex_match(url.utf8().data(), what, ex) &&
    std::string(what[1].first, what[1].second).length();
}

struct sURL {
  std::string protocol, domain, port, path, query;
};

static sURL parseURL(const WTF::String & url) {
  boost::regex ex(URL_REGEX);
  boost::cmatch what;
  if (regex_match(url.utf8().data(), what, ex)) {
    return {
      std::string(what[1].first, what[1].second), // protocol
      std::string(what[2].first, what[2].second), // domain
      std::string(what[3].first, what[3].second), // port
      std::string(what[4].first, what[4].second), // path
      std::string(what[5].first, what[5].second) // query
    };
  }
  throw NX::Exception("Invalid URL");
}

static size_t curlWriteCallback(char *ptr, size_t size, size_t nmemb, void * userdata) {
  if (userdata == nullptr)
    return 0;
  auto buf = reinterpret_cast<WTF::Vector<char>*>(userdata);
  buf->appendRange(ptr, ptr + size * nmemb);
  return size * nmemb;
}

static bool fetchModuleFromURL(const WTF::String &url, WTF::Vector<uint8_t> &buffer) {
  try {
    static thread_local bool init;
    CURLcode code;
    if (!init) {
      code = curl_global_init(CURL_GLOBAL_ALL);
      if (code != CURLE_OK)
        return false;
      init = true;
    }
    char errorBuf[CURL_ERROR_SIZE];
    CURL * session = curl_easy_init();
    if (!session)
      return false;
    std::string utf8URL(url.utf8().data());
    code = curl_easy_setopt(session, CURLOPT_ERRORBUFFER, errorBuf);
    if (code != CURLE_OK)
    {
      curl_easy_cleanup(session);
      return false;
    }
    code = curl_easy_setopt(session, CURLOPT_NOSIGNAL, 1L);
    if (code != CURLE_OK) {
      curl_easy_cleanup(session);
      return false;
    }
    code = curl_easy_setopt(session, CURLOPT_URL, utf8URL.c_str());
    if (code != CURLE_OK) {
      curl_easy_cleanup(session);
      return false;
    }
    code = curl_easy_setopt(session, CURLOPT_FOLLOWLOCATION, 1L);
    if (code != CURLE_OK) {
      curl_easy_cleanup(session);
      return false;
    }
    code = curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    if (code != CURLE_OK) {
      curl_easy_cleanup(session);
      return false;
    }
    code = curl_easy_setopt(session, CURLOPT_WRITEDATA, &buffer);
    if (code != CURLE_OK) {
      curl_easy_cleanup(session);
      return false;
    }
    code = curl_easy_perform(session);
    curl_easy_cleanup(session);
    if (code != CURLE_OK) {
      return false;
    }
    convertShebangToJSComment(buffer);
    return true;
  } catch (const std::exception &e) {
    NX::Nexus::ReportException(e);
    return false;
  }
}


JSInternalPromise *
NX::GlobalObject::moduleLoaderImportModule(JSGlobalObject *globalObject, ExecState *exec, JSModuleLoader *,
                                           JSString *moduleNameValue, JSValue parameters,
                                           const SourceOrigin &sourceOrigin) {
  VM &vm = globalObject->vm();
  auto scope = DECLARE_CATCH_SCOPE(vm);
  auto rejectPromise = [&](auto error) {
    return JSInternalPromiseDeferred::tryCreate(exec, globalObject)->reject(exec, error);
  };

  if (sourceOrigin.isNull())
    return rejectPromise(createError(exec, String("Could not resolve the module specifier.")));

  const auto &referrer = sourceOrigin.string();
  const auto &moduleName = moduleNameValue->value(exec);
  if (UNLIKELY(scope.exception())) {
    auto exception = scope.exception();
    scope.clearException();
    return rejectPromise(exception);
  }

  auto directoryName = extractDirectoryName(referrer.impl());
  if (!directoryName)
    return rejectPromise(
        createError(exec, makeString("Could not resolve the referrer name '", String(referrer.impl()), "'.")));
  auto result = JSC::importModule(exec, Identifier::fromString(&vm, resolvePath(directoryName.value(),
                                                                                ModuleName(moduleName))), parameters,
                                  jsUndefined());
  scope.releaseAssertNoException();
  return result;
}

Identifier
NX::GlobalObject::moduleLoaderResolve(JSGlobalObject *globalObject, ExecState *exec, JSModuleLoader *,
                                      JSValue keyValue, JSValue referrerValue, JSValue) {

  auto thisObject = reinterpret_cast<JSCallbackObject<NX::GlobalObject>*>(globalObject);
  auto & vm = thisObject->vm();

  auto scope = DECLARE_THROW_SCOPE(vm);

  scope.releaseAssertNoException();
  const Identifier key = keyValue.toPropertyKey(exec);
  RETURN_IF_EXCEPTION(scope, {});

  if (key.isSymbol())
    return key;

  if (referrerValue.isUndefined()) {
    auto directoryName = extractDirectoryName(WTF::String::fromUTF8(thisObject->nexus()->scriptPath().c_str()));
    if (!directoryName) {
      throwException(exec, scope, createError(exec, String("Could not resolve the current working directory.")));
      return {};
    }
    return Identifier::fromString(&vm, resolvePath(directoryName.value(), ModuleName(key.impl())));
  }

  const Identifier referrer = referrerValue.toPropertyKey(exec);
  RETURN_IF_EXCEPTION(scope, {});

  sURL url {};

  try {
    url = parseURL(key.impl());
  } catch (const std::exception & e) {
    url.path = key.impl()->utf8().data();
  }

//  std::cout << "protocol: " << url.protocol << std::endl;
//  std::cout << "domain: " << url.domain << std::endl;
//  std::cout << "path: " << url.path << std::endl;
//  std::cout << "port: " << url.port << std::endl;
//  std::cout << "query: " << url.query << std::endl;
//
//  if (!url.domain.empty() && url.domain != "." && url.domain != "..")
//  {
//    std::cout << "domain: " << url.domain << std::endl;
//  }

  // if the protocol is specified, pass it all along
  if (!url.protocol.empty())
    return Identifier::fromString(&vm, key.impl());

  // entry point
  if (referrer.isSymbol()) {
//    auto directoryName = currentWorkingDirectory();
    auto directoryName = extractDirectoryName(WTF::String::fromUTF8(thisObject->nexus()->scriptPath().c_str()));
    if (!directoryName) {
      throwException(exec, scope, createError(exec, String("Could not resolve the current working directory.")));
      return {};
    }
    return Identifier::fromString(&vm, resolvePath(directoryName.value(), ModuleName(
      boost::filesystem::path(url.domain).append(url.path).c_str())));
  }

  // If the referrer exists, we assume that the referrer is the correct absolute path.
  auto directoryName = extractDirectoryName(referrer.impl());
  if (!directoryName) {
    throwException(exec, scope, createError(exec,
                                            makeString("Could not resolve the referrer name '", String(referrer.impl()),
                                                       "'.")));
    return {};
  }

  String moduleKey = key.impl();

  return Identifier::fromString(&vm, resolvePath(directoryName.value(), ModuleName(
    boost::filesystem::path(url.domain).append(url.path).c_str())));

}

JSInternalPromise *
NX::GlobalObject::moduleLoaderFetch(JSGlobalObject *globalObject, ExecState *exec, JSModuleLoader *,
                                    JSValue key, JSValue, JSValue) {
  VM &vm = globalObject->vm();
  auto scope = DECLARE_CATCH_SCOPE(vm);
  JSInternalPromiseDeferred *deferred = JSInternalPromiseDeferred::tryCreate(exec, globalObject);
  String moduleKey = key.toWTFString(exec);

  if (UNLIKELY(scope.exception())) {
    auto exception = scope.exception();
    scope.clearException();
    return deferred->reject(exec, exception);
  }

  // Here, now we consider moduleKey as the fileName.
  WTF::Vector<uint8_t> buffer;
  JSC::SourceCode source;

  if (isURL(moduleKey)) {
    if (!fetchModuleFromURL(moduleKey, buffer)) {
      return deferred->reject(exec, createError(exec, makeString("Could not open URL '", moduleKey, "'.")));
    }
  } else if (!fetchModuleFromLocalFileSystem(moduleKey, buffer))
    return deferred->reject(exec, createError(exec, makeString("Could not open file '", moduleKey, "'.")));
  source = makeSource(stringFromUTF(buffer),
                      SourceOrigin {moduleKey}, 
                      // WTF::URL(moduleKey),
                      WTF::URL(),
                      TextPosition(),
                      SourceProviderSourceType::Module);
  JSC::ParserError error;
  if (!JSC::checkModuleSyntax(exec, source, error)) {
    NX::Nexus::ReportSyntaxError(source, error);
  }

  auto result = deferred->resolve(exec, JSSourceCode::create(vm, std::move(source)));
  scope.releaseAssertNoException();
  return result;
}

JSObject *
NX::GlobalObject::moduleLoaderCreateImportMetaProperties(JSGlobalObject *globalObject, ExecState *exec,
                                                         JSModuleLoader *,
                                                         JSValue key, JSModuleRecord *, JSValue) {
  VM &vm = exec->vm();
  auto scope = DECLARE_THROW_SCOPE(vm);

  JSObject *metaProperties = constructEmptyObject(exec, globalObject->nullPrototypeObjectStructure());
  RETURN_IF_EXCEPTION(scope, nullptr);

  metaProperties->putDirect(vm, Identifier::fromString(&vm, "filename"), key);
  RETURN_IF_EXCEPTION(scope, nullptr);

  metaProperties->putDirect(vm, Identifier::fromString(&vm, "url"), key);
  RETURN_IF_EXCEPTION(scope, nullptr);

  if (!key.isSymbol()) {
    auto path = boost::filesystem::path(key.toString(exec)->value(exec).utf8().data());
    path.remove_filename();
    NX::ScopedString dirNameStr(path.c_str());
    JSValueRef dirName = JSValueMakeString(toRef(exec), dirNameStr);
    metaProperties->putDirect(vm, Identifier::fromString(&vm, "dirname"), toJS(exec, dirName).toString(exec));
    RETURN_IF_EXCEPTION(scope, nullptr);
  }

  return metaProperties;
}

void NX::GlobalObject::promiseRejectionTracker(JSGlobalObject *jsGlobalObject, ExecState *exec, JSPromise *promise,
                                               JSPromiseRejectionOperation operation) {
  VM &vm = exec->vm();
//  if (JSC::jsDynamicCast<JSC::JSInternalPromise*>(vm, promise)) {
//    return;
//  }
//  auto &globalObject = *JSC::jsCast<JSCallbackObject<NX::GlobalObject> *>(jsGlobalObject);
  auto result = promise->result(vm);
  NX::Nexus::ReportException(toRef(exec), toRef(exec, result));
}

JSValue
NX::GlobalObject::moduleLoaderEvaluate(JSGlobalObject *global, ExecState *exec, JSModuleLoader *,
                                       JSC::JSValue moduleKey, JSC::JSValue moduleRecordValue,
                                       JSC::JSValue scriptFetcher)
{
  auto thisObject = reinterpret_cast<JSCallbackObject<NX::GlobalObject>*>(global);
//  auto parent = static_cast<NX::Context*>(thisObject->getPrivate());
//  auto context = new NX::Context(parent);
  JSLockHolder lockHolder(exec);
  auto moduleRecord = JSC::jsDynamicCast<JSC::JSModuleRecord*>(thisObject->vm(), moduleRecordValue);
  if (!moduleRecord) {
    return JSC::jsUndefined();
  }
  return moduleRecord->evaluate(exec);
}

