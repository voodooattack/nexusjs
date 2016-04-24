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
#include "util.h"
#include "value.h"
#include "object.h"
#include "context.h"
#include "scheduler.h"
#include "globals/promise.h"
#include "classes/io/device.h"

#include <boost/algorithm/string.hpp>

JSClassRef NX::Classes::IO::Device::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.className = "Device";
  def.staticValues = NX::Classes::IO::Device::Properties;
  def.staticFunctions = NX::Classes::IO::Device::Methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::SourceDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "SourceDevice";
  def.staticFunctions = NX::Classes::IO::SourceDevice::Methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::SinkDevice::createClass (Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "SinkDevice";
  def.staticFunctions = NX::Classes::IO::SourceDevice::Methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::BidirectionalDevice::createClass (Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "BidirectionalDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::SourceDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[0],
  };
  def.staticFunctions = methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::SeekableDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "SeekableDevice";
  def.staticFunctions = NX::Classes::IO::SeekableDevice::Methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::SeekableSourceDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "SeekableSourceDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::SeekableDevice::Methods[0],
    NX::Classes::IO::SeekableDevice::Methods[1],
    NX::Classes::IO::SourceDevice::Methods[0],
    NX::Classes::IO::SourceDevice::Methods[1],
  };
  def.staticFunctions = methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::SeekableSinkDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "SeekableSinkDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::SeekableDevice::Methods[0],
    NX::Classes::IO::SeekableDevice::Methods[1],
    NX::Classes::IO::SinkDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[1],
  };
  def.staticFunctions = methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::BidirectionalSeekableDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "BidirectionalSeekableDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::SourceDevice::Methods[0],
    NX::Classes::IO::SourceDevice::Methods[1],
    NX::Classes::IO::SinkDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[1],
    NX::Classes::IO::SeekableDevice::Methods[0],
    NX::Classes::IO::SeekableDevice::Methods[1],
  };
  def.staticFunctions = methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::DualSeekableDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "DualSeekableDevice";
  def.staticFunctions = NX::Classes::IO::DualSeekableDevice::Methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::BidirectionalDualSeekableDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "BidirectionalSeekableDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::DualSeekableDevice::Methods[0],
    NX::Classes::IO::DualSeekableDevice::Methods[1],
    NX::Classes::IO::DualSeekableDevice::Methods[2],
    NX::Classes::IO::DualSeekableDevice::Methods[3],
    NX::Classes::IO::SourceDevice::Methods[0],
    NX::Classes::IO::SourceDevice::Methods[1],
    NX::Classes::IO::SinkDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[2],
  };
  def.staticFunctions = methods;
  return context->nexus()->defineOrGetClass (def);
}

JSStaticValue NX::Classes::IO::Device::Properties[] {
  { "ready", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Classes::IO::Device * dev = NX::Classes::IO::Device::FromObject(object);
    return JSValueMakeBoolean(ctx, dev->deviceReady());
  }, nullptr, 0 },
  { nullptr, nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::Device::Methods[] {
//   { "lock", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
//     size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//       NX::Classes::IO::Device * dev = NX::Classes::IO::Device::FromObject(thisObject);
//       NX::Context * context = NX::Context::FromJsContext(ctx);
//       JSValueProtect(context->toJSContext(), thisObject);
//       JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), ScopedString("lock"),
//         [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
//           size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef
//       {
//         NX::Context * context = NX::Context::FromJsContext(ctx);
//         ctx = context->toJSContext();
//         NX::Classes::IO::Device * dev = NX::Classes::IO::Device::FromObject(thisObject);
//         try {
//           dev->deviceLock();
//           JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[0], exception), nullptr, 0, nullptr, exception);
//         } catch (const std::exception & e) {
//           NX::Value message(ctx, e.what());
//           JSValueRef args1[] { message.value(), nullptr };
//           JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
//           JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[1], exception), nullptr, 1, args2, exception);
//         }
//         JSValueUnprotect(ctx, thisObject);
//         return JSValueMakeUndefined(ctx);
//       }), thisObject, 0, nullptr, nullptr);
//       return NX::Globals::Promise::createPromise(context->toJSContext(), executor, exception);
//     }, 0
//   },
//   { "unlock", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
//     size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//       NX::Context * context = NX::Context::FromJsContext(ctx);
//       JSValueProtect(context->toJSContext(), thisObject);
//       JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), ScopedString("unlock"),
//         [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
//           size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception) -> JSValueRef
//       {
//         NX::Context * context = NX::Context::FromJsContext(ctx);
//         ctx = context->toJSContext();
//         NX::Classes::IO::Device * dev = NX::Classes::IO::Device::FromObject(thisObject);
//         try {
//           dev->deviceUnlock();
//           JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[0], exception), nullptr, 0, nullptr, exception);
//         } catch (const std::exception & e) {
//           NX::Value message(ctx, e.what());
//           JSValueRef args1[] { message.value(), nullptr };
//           JSValueRef args2[] { JSObjectMakeError(ctx, 1, args1, nullptr) };
//           JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[1], exception), nullptr, 1, args2, exception);
//         }
//         JSValueUnprotect(ctx, thisObject);
//         return JSValueMakeUndefined(ctx);
//       }), thisObject, 0, nullptr, nullptr);
//       return NX::Globals::Promise::createPromise(context->toJSContext(), executor, exception);
//     }, 0
//   },
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::SourceDevice::Methods[] {
  { "read", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      if (argumentCount == 0) {
        NX::Value message(ctx, "must supply length to read");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      } else {
        if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
          NX::Value message(ctx, "bad value for length argument");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
          return JSValueMakeUndefined(ctx);
        }
      }
      JSValueRef argsForBind[] { arguments[0] };
      JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
          size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::IO::SourceDevice * dev = NX::Classes::IO::SourceDevice::FromObject(thisObject);
        std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
        JSValueProtect(context->toJSContext(), thisObject);
        for(int i = 0; i < argumentCount; i++)
          JSValueProtect(context->toJSContext(), arguments[i]);
        boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
        std::size_t chunkSize = 4096;
        std::size_t length = JSValueToNumber(ctx, arguments[0], exception);
        scheduler->scheduleCoroutine([=]() {
          JSContextRef ctx = context->toJSContext();
          try {
            char * buffer = (char *)malloc(length);
            std::size_t readSoFar = 0;
            if(!dev->deviceReady())
              throw std::runtime_error("device not ready");
            for(std::size_t i = 0; i < length; i += chunkSize)
            {
              readSoFar += dev->deviceRead(buffer + i, std::min(chunkSize, length - i));
              scheduler->yield();
              if (dev->eof()) break;
            }
            if (readSoFar != length)
              buffer = (char *)realloc(buffer, readSoFar);
            scheduler->scheduleTask([=]() {
              JSValueRef exp = nullptr;
              JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, buffer, readSoFar,
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
      return NX::Globals::Promise::createPromise(context->toJSContext(), executor, exception);
    }, 0
  },
  { "readSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::IO::SourceDevice * dev = NX::Classes::IO::SourceDevice::FromObject(thisObject);
        if (argumentCount == 0) {
          throw std::runtime_error("must supply length to read");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
            throw std::runtime_error("bad value for length argument");
          }
        }
        std::size_t length = NX::Value(ctx, arguments[0]).toNumber();
        char * buffer = (char * )malloc(length);
        std::size_t readSoFar = 0;
        if(!dev->deviceReady())
          throw std::runtime_error("device not ready");
        readSoFar = dev->deviceRead(buffer, length);
        if (readSoFar != length)
          buffer = (char *)realloc(buffer, readSoFar);
        JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, buffer, readSoFar,
                [](void* bytes, void* deallocatorContext) { delete reinterpret_cast<char *>(bytes); }, nullptr, exception);
        return arrayBuffer;
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::SinkDevice::Methods[] {
  { "write", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      try {
        if (argumentCount == 0) {
          throw std::runtime_error("must supply buffer to write");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
            throw std::runtime_error("bad value for buffer argument");
          JSValueRef except = nullptr;
          NX::Object obj(ctx, arguments[0]);
          std::size_t length = JSObjectGetArrayBufferByteLength(ctx, obj.value(), &except);
          if (!except)
            arrayBuffer = obj.value();
          else {
            except = nullptr;
            arrayBuffer = JSObjectGetTypedArrayBuffer(ctx, obj.value(), &except);
            if (except) {
              throw std::runtime_error("argument must be TypedArray or ArrayBuffer");
            }
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      JSValueRef argsForBind[] { arrayBuffer };
      JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
          size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
        NX::Classes::IO::SinkDevice * dev = NX::Classes::IO::SinkDevice::FromObject(thisObject);
        JSValueProtect(context->toJSContext(), thisObject);
        for(int i = 0; i < argumentCount; i++)
          JSValueProtect(context->toJSContext(), arguments[i]);
        boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
        std::size_t chunkSize = 4096;
        JSObjectRef arrayBuffer = NX::Object(ctx, arguments[0]).value();
        scheduler->scheduleCoroutine([=]() {
          JSContextRef ctx = context->toJSContext();
          try {
            const char * buffer = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, nullptr);
            std::size_t length = JSObjectGetArrayBufferByteLength(ctx, arrayBuffer, nullptr);
            if(!dev->deviceReady())
              throw std::runtime_error("device not ready");
            for(std::size_t i = 0; i < length; i += chunkSize)
            {
              dev->deviceWrite(buffer + i, std::min(chunkSize, length - i));
              scheduler->yield();
            }
            scheduler->scheduleTask([=]() {
              JSValueRef args[] { thisObject };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 1, args, exception);
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
      return NX::Globals::Promise::createPromise(context->toJSContext(), executor, exception);
    }, 0
  },
  { "writeSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      try {
        if (argumentCount == 0) {
          throw std::runtime_error("must supply buffer to write");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
            throw std::runtime_error("bad value for buffer argument");
          JSValueRef except = nullptr;
          NX::Object obj(ctx, arguments[0]);
          std::size_t length = JSObjectGetArrayBufferByteLength(ctx, obj.value(), &except);
          if (!except)
            arrayBuffer = obj.value();
          else {
            except = nullptr;
            arrayBuffer = JSObjectGetTypedArrayBuffer(ctx, obj.value(), &except);
            if (except) {
              throw std::runtime_error("argument must be TypedArray or ArrayBuffer");
            }
          }
        }
        NX::Classes::IO::SinkDevice * dev = NX::Classes::IO::SinkDevice::FromObject(thisObject);
        const char * buffer = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, exception);
        std::size_t length = JSObjectGetArrayBufferByteLength(ctx, arrayBuffer, exception);
        if(!dev->deviceReady())
          throw std::runtime_error("device not ready");
        dev->deviceWrite(buffer, length);
        return JSValueMakeNumber(ctx, length);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::SeekableDevice::Methods[] {
  { "seek", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw std::runtime_error("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw std::runtime_error("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw std::runtime_error("value for position argument is not a string");
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw std::runtime_error("position argument must be one of [begin,current,end]");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      JSValueRef argsForBind[] { arguments[0], arguments[1] };
      JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
          size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
        NX::Classes::IO::SeekableDevice * dev = NX::Classes::IO::SeekableDevice::FromObject(thisObject);
        boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
        std::size_t offset = NX::Value (ctx, arguments[0]).toNumber();
        std::string position = NX::Value (ctx, arguments[1]).toString();
        scheduler->scheduleCoroutine([=]() {
          JSContextRef ctx = context->toJSContext();
          try {
            Device::Position pos;
            if (boost::iequals(position, "begin"))
              pos = Beginning;
            else if (boost::iequals(position, "current"))
              pos = Current;
            else if (boost::iequals(position, "end"))
              pos = End;
            if(!dev->deviceReady())
              throw std::runtime_error("device not ready");
            std::size_t newOffset = dev->deviceSeek(offset, pos);
            scheduler->scheduleTask([=]() {
              JSValueRef args[] { thisObject, NX::Value(ctx, newOffset).value() };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 2, args, exception);
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
      return NX::Globals::Promise::createPromise(context->toJSContext(), executor, exception);
    }, 0
  },
  { "seekSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw std::runtime_error("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw std::runtime_error("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw std::runtime_error("value for position argument is not a string");
          NX::Value position(ctx, arguments[0]);
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw std::runtime_error("position argument must be one of [begin,current,end]");
          }
          NX::Classes::IO::SeekableDevice * dev = NX::Classes::IO::SeekableDevice::FromObject(thisObject);
          Device::Position pos;
          if (boost::iequals(position.toString(), "begin"))
            pos = Beginning;
          else if (boost::iequals(position.toString(), "current"))
            pos = Current;
          else if (boost::iequals(position.toString(), "end"))
            pos = End;
          if (!dev->deviceReady())
            throw std::runtime_error("device not ready");
          std::size_t newOffset = dev->deviceSeek(offset.toNumber(), pos);
          return NX::Value(ctx, newOffset).value();
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::DualSeekableDevice::Methods[] {
  { "readSeek", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw std::runtime_error("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw std::runtime_error("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw std::runtime_error("value for position argument is not a string");
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw std::runtime_error("position argument must be one of [begin,current,end]");
          }
        }
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      JSValueRef argsForBind[] { arguments[0], arguments[1] };
      JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
          size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
        NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
        boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
        scheduler->scheduleCoroutine([=]() {
          JSContextRef ctx = context->toJSContext();
          try {
            NX::Value offset(ctx, arguments[0]);
            NX::Value position(ctx, arguments[1]);
            Device::Position pos;
            if (boost::iequals(position.toString(), "begin"))
              pos = Beginning;
            else if (boost::iequals(position.toString(), "current"))
              pos = Current;
            else if (boost::iequals(position.toString(), "end"))
              pos = End;
            if(!dev->deviceReady())
              throw std::runtime_error("device not ready");
            std::size_t newOffset = dev->deviceReadSeek(offset.toNumber(), pos);
            scheduler->scheduleTask([=]() {
              JSValueRef args[] { thisObject, NX::Value(ctx, newOffset).value() };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 2, args, exception);
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
      return NX::Globals::Promise::createPromise(context->toJSContext(), executor, exception);
    }, 0
  },
  { "writeSeek", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw std::runtime_error("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw std::runtime_error("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw std::runtime_error("value for position argument is not a string");
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw std::runtime_error("position argument must be one of [begin,current,end]");
          }
        }
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      JSValueRef argsForBind[] { arguments[0], arguments[1] };
      JSObjectRef executor = JSBindFunction(context->toJSContext(), JSObjectMakeFunctionWithCallback(context->toJSContext(), nullptr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
          size_t argumentCount, const JSValueRef originalArguments[], JSValueRef * exception) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        std::vector<JSValueRef> arguments = std::vector<JSValueRef>(originalArguments, originalArguments + argumentCount);
        NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
        boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
        scheduler->scheduleCoroutine([=]() {
          JSContextRef ctx = context->toJSContext();
          try {
            NX::Value offset(ctx, arguments[0]);
            NX::Value position(ctx, arguments[1]);
            Device::Position pos;
            if (boost::iequals(position.toString(), "begin"))
              pos = Beginning;
            else if (boost::iequals(position.toString(), "current"))
              pos = Current;
            else if (boost::iequals(position.toString(), "end"))
              pos = End;
            if(!dev->deviceReady())
              throw std::runtime_error("device not ready");
            std::size_t newOffset = dev->deviceWriteSeek(offset.toNumber(), pos);
            scheduler->scheduleTask([=]() {
              JSValueRef args[] { thisObject, NX::Value(ctx, newOffset).value() };
              JSObjectCallAsFunction(ctx, JSValueToObject(ctx, arguments[argumentCount - 2], exception), nullptr, 2, args, exception);
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
      return NX::Globals::Promise::createPromise(context->toJSContext(), executor, exception);
    }, 0
  },
  { "readSeekSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw std::runtime_error("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw std::runtime_error("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw std::runtime_error("value for position argument is not a string");
          NX::Value position(ctx, arguments[0]);
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw std::runtime_error("position argument must be one of [begin,current,end]");
          }
          NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
          Device::Position pos;
          if (boost::iequals(position.toString(), "begin"))
            pos = Beginning;
          else if (boost::iequals(position.toString(), "current"))
            pos = Current;
          else if (boost::iequals(position.toString(), "end"))
            pos = End;
          if(!dev->deviceReady())
            throw std::runtime_error("device not ready");
          std::size_t newOffset = dev->deviceReadSeek(offset.toNumber(), pos);
          return NX::Value(ctx, newOffset).value();
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "writeSeekSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw std::runtime_error("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw std::runtime_error("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw std::runtime_error("value for position argument is not a string");
          NX::Value position(ctx, arguments[0]);
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw std::runtime_error("position argument must be one of [begin,current,end]");
          }
          NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
          Device::Position pos;
          if (boost::iequals(position.toString(), "begin"))
            pos = Beginning;
          else if (boost::iequals(position.toString(), "current"))
            pos = Current;
          else if (boost::iequals(position.toString(), "end"))
            pos = End;
          if(!dev->deviceReady())
            throw std::runtime_error("device not ready");
          std::size_t newOffset = dev->deviceWriteSeek(offset.toNumber(), pos);
          return NX::Value(ctx, newOffset).value();
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};
