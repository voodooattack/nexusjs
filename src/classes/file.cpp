#include "module.h"
#include "value.h"
#include "object.h"
#include "nexus.h"
#include "scheduler.h"
#include "util.h"
#include "classes/stream.h"
#include "classes/file.h"
#include "globals/promise.h"

#include <iconv.h>
#include <errno.h>

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
  JSObjectRef executor = JSBindFunction(module->context(), JSObjectMakeFunctionWithCallback(module->context(), nullptr,
      [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
    {
      NX::Module * module = Module::FromContext(ctx);
      std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
      JSValueProtect(module->context(), thisObject);
      for(int i = 0; i < argumentCount; i++)
        JSValueProtect(module->context(), arguments[i]);
      NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
      boost::shared_ptr<NX::Scheduler> scheduler = module->nexus()->scheduler();
      std::size_t chunkSize = 4096;
      std::size_t length = 0;
      std::fstream & is(file->myStream);
      if (argumentCount >= 1)
      {
        length = NX::Value(ctx, arguments[0]).toNumber();
        is.seekg (0, std::ios_base::end);
        length = std::min(length, (std::size_t)is.tellg());
        is.seekg (0, std::ios_base::beg);
      } else {
        is.seekg (0, std::ios_base::end);
        length = is.tellg();
        is.seekg (0, std::ios_base::beg);
      }
      scheduler->scheduleCoroutine([=]() {
        JSContextRef ctx = module->context();
        try {
          std::fstream & is(file->myStream);
          char * buffer = new char[length];
          for(std::size_t i = 0; i < length; i += chunkSize)
          {
            is.read (buffer + i, std::min(chunkSize, length - i));
            scheduler->yield();
          }
          scheduler->scheduleTask([=]() {
            JSValueRef exp = nullptr;
            JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, buffer, length,
              [](void* bytes, void* deallocatorContext) { delete reinterpret_cast<char *>(bytes); }, nullptr, &exp);
            JSObjectRef array = nullptr;
            if (!exp)
              array = JSObjectMakeTypedArrayWithArrayBuffer(ctx, kJSTypedArrayTypeInt8Array, arrayBuffer, &exp);
            if (exp)
            {
              JSValueRef args[] { exp };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args, exception);
            } else {
              JSValueRef args[] { array };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 1, args, exception);
            }
            for(int i = 0; i < argumentCount; i++)
              JSValueUnprotect(module->context(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        } catch (const std::exception & e) {
          scheduler->scheduleTask([=]() {
            NX::Value message(ctx, e.what());
            JSValueRef args1[] { message.value(), nullptr };
            JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
            JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
            for(int i = 0; i < argumentCount; i++)
              JSValueUnprotect(module->context(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        }
      });
      return JSValueMakeUndefined(ctx);
    }), thisObject, argumentCount, arguments, exception);
  if (exception && *exception)
    return JSValueMakeUndefined(ctx);
  JSObjectRef promise = NX::Globals::Promise::createPromise(ctx, executor, exception);
  return promise;
}

JSValueRef NX::Classes::File::readAsString (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                                    size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Module * module = Module::FromContext(ctx);
  JSObjectRef executor = JSBindFunction(module->context(), JSObjectMakeFunctionWithCallback(module->context(), nullptr,
      [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
    {
      NX::Module * module = Module::FromContext(ctx);
      std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
      JSValueProtect(module->context(), thisObject);
      for(int i = 0; i < argumentCount; i++)
        JSValueProtect(module->context(), arguments[i]);
      NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
      boost::shared_ptr<NX::Scheduler> scheduler = module->nexus()->scheduler();
      std::size_t chunkSize = 4096;
      std::size_t length = 0;
      std::fstream & is(file->myStream);
      std::string encoding = "UTF8";
      if (argumentCount >= 3) {
        try {
          encoding = NX::Value(ctx, arguments[0]).toString();
        } catch(const std::exception & e) {
          NX::Value message(ctx, e.what());
          JSValueRef args1[] { message.value(), nullptr };
          JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
          JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
          for(int i = 0; i < argumentCount; i++)
            JSValueUnprotect(module->context(), arguments[i]);
          JSValueUnprotect(ctx, thisObject);
          return JSValueMakeUndefined(ctx);
        }
      }
      if (argumentCount >= 4 && JSValueGetType(ctx, arguments[1]) == kJSTypeNumber)
      {
        length = NX::Value(ctx, arguments[1]).toNumber();
        is.seekg (0, std::ios_base::end);
        length = std::min(length, (std::size_t)is.tellg());
        is.seekg (0, std::ios_base::beg);
      } else {
        is.seekg (0, std::ios_base::end);
        length = is.tellg();
        is.seekg (0, std::ios_base::beg);
      }
      scheduler->scheduleCoroutine([=]() {
        JSContextRef ctx = module->context();
        try {
          std::fstream & is(file->myStream);
          iconv_t cd = iconv_open("WCHAR_T", encoding.c_str());
          if (cd == (iconv_t) -1)
          {
            scheduler->scheduleTask([=]() {
              std::string err("conversion from '" + encoding + "' to unicode not possible.");
              NX::Value message(ctx, err);
              JSValueRef args1[] { message.value(), nullptr };
              JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
              for(int i = 0; i < argumentCount; i++)
                JSValueUnprotect(module->context(), arguments[i]);
              JSValueUnprotect(ctx, thisObject);
            });
            return;
          }
          std::string buffer(length, 0);
          std::wstring out(length * 2, 0);
          size_t outWritten = 0;
          for(std::size_t i = 0; i < length; i += chunkSize)
          {
            char * pointer = &buffer[0] + i;
            is.read (pointer, std::min(chunkSize, length - i));
            size_t justRead = is.gcount();
            while(1) {
              errno = 0;
              size_t outBytesLeft = out.size() * 2 - outWritten;
              size_t outBytesBeforeWriting = outBytesLeft;
              char * outBuf = (char*)(&out[0]) + outWritten;
              size_t value = 0;
              value = iconv(cd, &pointer, &justRead, &outBuf, &outBytesLeft);
              if (value == (size_t) -1) {
                if (errno == E2BIG) {
                  out.resize(out.size() + outBytesLeft / 2, 0);
                } else {
                  break;
                }
              } else {
                outWritten += outBytesBeforeWriting - outBytesLeft;
                break;
              }
            };
            scheduler->yield();
          }
          out.resize(outWritten / 2);
          iconv_close(cd);
          scheduler->scheduleTask([=]() {
            JSValueRef exp = nullptr;
            JSStringRef stringBuffer = JSStringCreateWithCharacters((const JSChar *)out.c_str(), out.size() * 2);
            JSValueRef string = JSValueMakeString(ctx, stringBuffer);
            JSStringRelease(stringBuffer);
            if (exp)
            {
              JSValueRef args[] { exp };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args, exception);
            } else {
              JSValueRef args[] { string };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 1, args, exception);
            }
            for(int i = 0; i < argumentCount; i++)
              JSValueUnprotect(module->context(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        } catch (const std::exception & e) {
          scheduler->scheduleTask([=]() {
            NX::Value message(ctx, e.what());
            JSValueRef args1[] { message.value(), nullptr };
            JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
            JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
            for(int i = 0; i < argumentCount; i++)
              JSValueUnprotect(module->context(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        }
      });
      return JSValueMakeUndefined(ctx);
    }), thisObject, argumentCount, arguments, exception);
  if (exception && *exception)
    return JSValueMakeUndefined(ctx);
  JSObjectRef promise = NX::Globals::Promise::createPromise(ctx, executor, exception);
  return promise;
}

// JSValueRef NX::Classes::File::readAsString (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
//                                             size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
// {
//   NX::Module * module = Module::FromContext(ctx);
//   JSValueProtect(ctx, thisObject);
//   JSObjectRef executor = JSBindFunction(ctx, JSObjectMakeFunctionWithCallback(ctx, nullptr,
//       [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
//       size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef
//     {
//       NX::Module * module = Module::FromContext(ctx);
//       NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
//
//       JSValueRef exp = nullptr;
//       try {
//         char * buffer = nullptr;
//         std::size_t length = 0;
//         std::fstream & is(file->myStream);
//         if (argumentCount >= 1)
//         {
//           length = NX::Value(ctx, arguments[0]).toNumber();
//           is.seekg (0, std::ios_base::end);
//           length = std::min(length, (std::size_t)is.tellg());
//           is.seekg (0, std::ios_base::beg);
//         } else {
//           is.seekg (0, std::ios_base::end);
//           length = is.tellg();
//           is.seekg (0, std::ios_base::beg);
//         }
//         buffer = new char [length];
//         is.read (buffer, length);
//         std::wstring dest(buffer, buffer + length - 1);
//         delete buffer;
//         JSStringRef stringBuffer = JSStringCreateWithCharacters((const JSChar *)dest.c_str(), dest.length() * 2);
//         JSValueRef string = JSValueMakeString(ctx, stringBuffer);
//         JSStringRelease(stringBuffer);
//         JSValueUnprotect(ctx, thisObject);
//         if (exp)
//         {
//           JSValueRef args[] { exp };
//           return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args, exception);
//         } else {
//           JSValueRef args[] { string };
//           return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 1, args, exception);
//         }
//       } catch (const std::exception & e) {
//         NX::Value message(ctx, e.what());
//         JSValueRef args1[] { message.value(), nullptr };
//         JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
//         return JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[1], exception), nullptr, 1, args2, exception);
//       }
//     }), thisObject, argumentCount, arguments, exception);
//   if (exception && *exception)
//     return JSValueMakeUndefined(ctx);
//   JSObjectRef promise = NX::Globals::Promise::createPromise(ctx, executor, exception);
//   return promise;
// }


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
