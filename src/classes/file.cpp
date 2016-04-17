#include "module.h"
#include "value.h"
#include "object.h"

#include "classes/stream.h"
#include "classes/file.h"
#include "globals/promise.h"
#include "util.h"

JSClassRef NX::Classes::File::createClass (NX::Module * module)
{
  JSClassRef Stream = NX::Classes::Stream::createClass(module);
  JSClassDefinition FileDef = NX::Classes::File::Class;
  FileDef.parentClass = Stream;
  return module->defineOrGetClass(FileDef);
}

JSObjectRef NX::Classes::File::getConstructor (NX::Module * module)
{
  return JSObjectMakeConstructor(module->context(), NX::Classes::File::createClass(module), NX::Classes::File::Constructor);
}

JSObjectRef NX::Classes::File::Constructor (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                            const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Module * module = Module::FromContext(ctx);
  JSClassRef fileClass = createClass(module);
  if (argumentCount != 2) {
    NX::Value message(ctx, "invalid arguments");
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
  try {
    /* It is important we cast to Stream here, the stream finalizer expects a Stream pointer, not a File pointer. */
    return JSObjectMake(ctx, fileClass, dynamic_cast<NX::Classes::Stream*>(new NX::Classes::File(
      module,
      NX::Value(ctx, arguments[0]).toString(),
      (std::iostream::openmode)NX::Value(ctx, arguments[1]).toNumber()
    )));
  } catch(const std::exception & e) {
    NX::Value message(ctx, e.what());
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  }
  return JSObjectMake(ctx, nullptr, nullptr);
}

NX::Classes::File::File (NX::Module * owner, const std::string & fileName,
                         std::fstream::openmode mode):
  myOwner (owner), myStream (fileName, mode)
{

}

JSValueRef NX::Classes::File::readAsBuffer (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                                    size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Module * module = Module::FromContext(ctx);
  JSValueProtect(ctx, thisObject);
  JSObjectRef executor =JSBindFunction(ctx, JSObjectMakeFunctionWithCallback(ctx, nullptr,
      [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef
    {
      NX::Module * module = Module::FromContext(ctx);
      NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
      JSValueRef exp = nullptr;
      try {
        std::fstream & is(file->myStream);
        is.seekg (0, std::ios_base::end);
        std::size_t length = is.tellg();
        is.seekg (0, std::ios_base::beg);
        char * buffer = new char [length];
        is.read (buffer, length);
        JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, buffer, length,
          [](void* bytes, void* deallocatorContext) { delete reinterpret_cast<char *>(bytes); }, nullptr, &exp);
        JSObjectRef array = nullptr;
        if (!exp)
          array = JSObjectMakeTypedArrayWithArrayBuffer(ctx, kJSTypedArrayTypeInt8Array, arrayBuffer, &exp);
        JSValueUnprotect(ctx, thisObject);
        if (exp)
        {
          JSValueRef args[] { exp };
          return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[1], exception), nullptr, 1, args, exception);
        } else {
          JSValueRef args[] { array };
          return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[0], exception), nullptr, 1, args, exception);
        }
      } catch (const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args1[] { message.value(), nullptr };
        JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
        return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[1], exception), nullptr, 1, args2, exception);
      }
    }), thisObject, 0, nullptr, exception);
  if (exception && *exception)
    return JSValueMakeUndefined(ctx);
  JSObjectRef promise = NX::Globals::Promise::createPromise(ctx, executor, exception);
  return promise;
}

JSValueRef NX::Classes::File::readAsString (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Module * module = Module::FromContext(ctx);
  JSValueProtect(ctx, thisObject);
  JSObjectRef executor =JSBindFunction(ctx, JSObjectMakeFunctionWithCallback(ctx, nullptr,
      [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef
    {
      NX::Module * module = Module::FromContext(ctx);
      NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
      JSValueRef exp = nullptr;
      try {
        std::fstream & is(file->myStream);
        is.seekg (0, std::ios_base::end);
        std::size_t length = is.tellg();
        is.seekg (0, std::ios_base::beg);
        char * buffer = new char [length];
        is.read (buffer, length);
        std::wstring dest(buffer, buffer + length);
        delete buffer;
        JSStringRef stringBuffer = JSStringCreateWithCharacters((const JSChar *)dest.c_str(), dest.length() * 2);
        JSValueRef string = JSValueMakeString(ctx, stringBuffer);
        JSStringRelease(stringBuffer);
        JSValueUnprotect(ctx, thisObject);
        if (exp)
        {
          JSValueRef args[] { exp };
          return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[1], exception), nullptr, 1, args, exception);
        } else {
          JSValueRef args[] { string };
          return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[0], exception), nullptr, 1, args, exception);
        }
      } catch (const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args1[] { message.value(), nullptr };
        JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
        return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[1], exception), nullptr, 1, args2, exception);
      }
    }), thisObject, 0, nullptr, exception);
  if (exception && *exception)
    return JSValueMakeUndefined(ctx);
  JSObjectRef promise = NX::Globals::Promise::createPromise(ctx, executor, exception);
  return promise;
}


const JSClassDefinition NX::Classes::File::Class {
  0, kJSClassAttributeNone, "File", nullptr, NX::Classes::File::Properties,
  NX::Classes::File::Methods, nullptr, NX::Classes::File::Finalize
};

const JSStaticValue NX::Classes::File::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::File::Methods[] {
  { "readAsBuffer", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = Module::FromContext(ctx);
      NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
      if (!file) {
        NX::Value message(ctx, "invalid File instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      return file->readAsBuffer(ctx, function, thisObject, argumentCount, arguments, exception);
    }, 0
  },
  { "readAsString", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = Module::FromContext(ctx);
      NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
      if (!file) {
        NX::Value message(ctx, "invalid File instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      return file->readAsString(ctx, function, thisObject, argumentCount, arguments, exception);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
