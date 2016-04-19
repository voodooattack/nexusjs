#include "context.h"
#include "value.h"
#include "object.h"
#include "nexus.h"
#include "scheduler.h"
#include "util.h"
#include "classes/stream.h"
#include "classes/file.h"
#include "globals/promise.h"

#include <boost/filesystem.hpp>

#include <iconv.h>
#include <errno.h>

JSClassRef NX::Classes::File::createClass (NX::Context * context)
{
  JSClassRef Stream = NX::Classes::Stream::createClass(context);
  JSClassDefinition FileDef = NX::Classes::File::Class;
  FileDef.parentClass = Stream;
  return context->defineOrGetClass(FileDef);
}

JSObjectRef NX::Classes::File::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), NX::Classes::File::createClass(context), NX::Classes::File::Constructor);
}

JSObjectRef NX::Classes::File::Constructor (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                            const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  JSClassRef fileClass = createClass(context);
  if (argumentCount != 2) {
    NX::Value message(ctx, "invalid arguments");
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
  try {
    /* It is important we cast to Stream here, the stream finalizer expects a Stream pointer, not a File pointer. */
    return JSObjectMake(ctx, fileClass, dynamic_cast<NX::Classes::Stream*>(new NX::Classes::File(
                           context,
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

NX::Classes::File::File (NX::Context * owner, const std::string & fileName,
                         std::fstream::openmode mode):
  myOwner (owner), myStream (fileName, mode)
{
  if (!boost::filesystem::exists(fileName))
    throw std::runtime_error("file not found");
  myStream.seekg(0, std::ios_base::beg);
}

JSValueRef NX::Classes::File::readAsBuffer (JSContextRef ctx, JSObjectRef thisObject, std::size_t length)
{
  NX::Context * context = Context::FromJsContext(ctx);
  std::fstream & is(myStream);
  if (length == (std::size_t)-1) {
    std::streampos pos = is.tellg();
    is.seekg (0, std::ios_base::end);
    length = is.tellg() - pos;
    is.seekg (pos, std::ios_base::beg);
  }
  JSValueRef argsForBind[] { JSValueMakeNumber(ctx, length) };
  JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
      [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
    {
      NX::Context * context = Context::FromJsContext(ctx);
      std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
      NX:Classes::File * file = NX::Classes::File::FromObject(thisObject);
      JSValueProtect(context->toJSContext(), thisObject);
      for(int i = 0; i < argumentCount; i++)
        JSValueProtect(context->toJSContext(), arguments[i]);
      boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
      std::size_t chunkSize = 4096;
      std::size_t length = JSValueToNumber(ctx, arguments[0], exception);
      scheduler->scheduleCoroutine([=]() {
        JSContextRef ctx = context->toJSContext();
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
            if (exp)
            {
              JSValueRef args[] { exp };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args, exception);
            } else {
              JSValueRef args[] { arrayBuffer };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 1, args, exception);
            }
            for(int i = 0; i < argumentCount; i++)
              JSValueUnprotect(context->toJSContext(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        } catch (const std::exception & e) {
          scheduler->scheduleTask([=]() {
            NX::Value message(ctx, e.what());
            JSValueRef args1[] { message.value(), nullptr };
            JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
            JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
            for(int i = 0; i < argumentCount; i++)
              JSValueUnprotect(context->toJSContext(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        }
      });
      return JSValueMakeUndefined(ctx);
    }), thisObject, 1, argsForBind, nullptr);
  JSObjectRef promise = NX::Globals::Promise::createPromise(ctx, executor, nullptr);
  return promise;
}

JSValueRef NX::Classes::File::readAsString (JSContextRef ctx, JSObjectRef thisObject, const std::string & encoding, std::size_t length)
{
  NX::Context * context = Context::FromJsContext(ctx);
  std::fstream & is(myStream);
  if (length == (std::size_t)-1) {
    std::streampos pos = is.tellg();
    is.seekg (0, std::ios_base::end);
    length = is.tellg() - pos;
    is.seekg (pos, std::ios_base::beg);
  }
  JSValueRef argsForBind[] { NX::Value(ctx, encoding).value(), JSValueMakeNumber(ctx, length) };
  JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
      [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
    {
      NX::Context * context = Context::FromJsContext(ctx);
      std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
      JSValueProtect(context->toJSContext(), thisObject);
      for(int i = 0; i < argumentCount; i++)
        JSValueProtect(context->toJSContext(), arguments[i]);
      NX::Classes::File * file = NX::Classes::File::FromObject(thisObject);
      boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
      std::size_t chunkSize = 4096;
      std::size_t length = 0;
      std::fstream & is(file->myStream);
      std::string encoding;
      if (argumentCount >= 3) {
        try {
          encoding = NX::Value(ctx, arguments[0]).toString();
        } catch(const std::exception & e) {
          NX::Value message(ctx, e.what());
          JSValueRef args1[] { message.value(), nullptr };
          JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
          JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
          for(int i = 0; i < argumentCount; i++)
            JSValueUnprotect(context->toJSContext(), arguments[i]);
          JSValueUnprotect(ctx, thisObject);
          return JSValueMakeUndefined(ctx);
        }
      }
      if (argumentCount >= 4 && JSValueGetType(ctx, arguments[1]) == kJSTypeNumber)
      {
        length = NX::Value(ctx, arguments[1]).toNumber();
        std::fstream::pos_type pos = is.tellg();
        is.seekg (0, std::ios_base::end);
        length = std::min(length, (std::size_t)is.tellg() - pos);
        is.seekg (pos, std::ios_base::beg);
      } else {
        std::fstream::pos_type pos = is.tellg();
        is.seekg (0, std::ios_base::end);
        length = is.tellg() - pos;
        is.seekg (pos, std::ios_base::beg);
      }
      scheduler->scheduleCoroutine([=]() {
        JSContextRef ctx = context->toJSContext();
        try {
          std::fstream & is(file->myStream);
          iconv_t cd = iconv_open("UTF8", encoding.c_str());
          if (cd == (iconv_t) -1)
          {
            scheduler->scheduleTask([=]() {
              std::string err("conversion from '" + encoding + "' to unicode not possible.");
              NX::Value message(ctx, err);
              JSValueRef args1[] { message.value(), nullptr };
              JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
              for(int i = 0; i < argumentCount; i++)
                JSValueUnprotect(context->toJSContext(), arguments[i]);
              JSValueUnprotect(ctx, thisObject);
            });
            return;
          }
          std::string buffer(length, 0);
          std::string out(length, 0);
          size_t outWritten = 0;
          for(std::size_t i = 0; i < length; i += chunkSize)
          {
            char * pointer = &buffer[0] + i;
            is.read (pointer, std::min(chunkSize, length - i));
            size_t justRead = is.gcount();
            while(1) {
              errno = 0;
              size_t outBytesLeft = out.size() - outWritten;
              size_t outBytesBeforeWriting = outBytesLeft;
              char * outBuf = (char*)(&out[0]) + outWritten;
              size_t value = 0;
              value = iconv(cd, &pointer, &justRead, &outBuf, &outBytesLeft);
              if (value == (size_t) -1) {
                if (errno == E2BIG) {
                  out.resize(out.size() + outBytesLeft, 0);
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
          out.resize(outWritten);
          iconv_close(cd);
          scheduler->scheduleTask([=]() {
            JSValueRef exp = nullptr;
            JSStringRef stringBuffer = JSStringCreateWithUTF8CString(out.c_str());
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
              JSValueUnprotect(context->toJSContext(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        } catch (const std::exception & e) {
          scheduler->scheduleTask([=]() {
            NX::Value message(ctx, e.what());
            JSValueRef args1[] { message.value(), nullptr };
            JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
            JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 1], exception), nullptr, 1, args2, exception);
            for(int i = 0; i < argumentCount; i++)
              JSValueUnprotect(context->toJSContext(), arguments[i]);
            JSValueUnprotect(ctx, thisObject);
          });
        }
      });
      return JSValueMakeUndefined(ctx);
    }), thisObject, 2, argsForBind, nullptr);
  JSObjectRef promise = NX::Globals::Promise::createPromise(ctx, executor, nullptr);
  return promise;
}

JSValueRef NX::Classes::File::readAsBufferSync (JSContextRef ctx, JSObjectRef thisObject, std::size_t length, JSValueRef * exception)
{
  std::fstream & is(myStream);
  if (length == (std::size_t)-1) {
    std::fstream::pos_type pos = is.tellg();
    is.seekg (0, std::ios_base::end);
    length = is.tellg() - pos;
    is.seekg (pos, std::ios_base::beg);
  }
  JSValueRef ret = nullptr;
  try {
    char * buffer = new char[length];
    is.read(buffer, length);
    length = is.gcount();
    ret = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, buffer, length,
          [](void* bytes, void* deallocatorContext) { delete reinterpret_cast<char *>(bytes); }, nullptr, exception);
  } catch(const std::exception & e) {
    NX::Value message(ctx, e.what());
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  }
  return ret ? ret : JSValueMakeUndefined(ctx);
}

JSValueRef NX::Classes::File::readAsStringSync (JSContextRef ctx, JSObjectRef thisObject, const std::__cxx11::string & encoding,
                                                std::size_t length, JSValueRef * exception)
{
  std::fstream & is(myStream);
  if (length == (std::size_t)-1) {
    std::streampos pos = is.tellg();
    is.seekg (0, std::ios_base::end);
    length = std::min(length, (std::size_t)is.tellg() - pos);
    is.seekg (pos, std::ios_base::beg);
  }
  JSValueRef ret = nullptr;
  try {
    std::string buffer;
    std::string out;
    buffer.resize(length);
    out.resize(length);
    is.read(&buffer[0], length);
    iconv_t cd = iconv_open("UTF8", encoding.c_str());
    if (cd == (iconv_t) -1)
    {
      throw std::runtime_error("conversion from '" + encoding + "' to unicode not possible.");
    }
    size_t justRead = is.gcount(), outWritten = 0;
    buffer.resize(justRead);
    while(1) {
      errno = 0;
      size_t outBytesLeft = out.size() - outWritten;
      size_t outBytesBeforeWriting = outBytesLeft;
      char * outBuf = (char*)(&out[0]) + outWritten;
      size_t value = 0;
      char * pointer = &buffer[0];
      value = iconv(cd, &pointer, &justRead, &outBuf, &outBytesLeft);
      if (value == (size_t) -1) {
        if (errno == E2BIG) {
          out.resize(out.size() + outBytesLeft, 0);
        } else {
          break;
        }
      } else {
        outWritten += outBytesBeforeWriting - outBytesLeft;
        break;
      }
    };
    out.resize(outWritten);
    iconv_close(cd);
    JSStringRef stringBuffer = JSStringCreateWithUTF8CString(out.c_str());
    ret = JSValueMakeString(ctx, stringBuffer);
    JSStringRelease(stringBuffer);
  } catch(const std::exception & e) {
    NX::Value message(ctx, e.what());
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  }
  return ret ? ret : JSValueMakeUndefined(ctx);
}


const JSClassDefinition NX::Classes::File::Class {
  0, kJSClassAttributeNone, "File", nullptr, NX::Classes::File::Properties,
  NX::Classes::File::Methods, nullptr, NX::Classes::File::Finalize
};

const JSStaticValue NX::Classes::File::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::File::Methods[] {
  { nullptr, nullptr, 0 }
};
