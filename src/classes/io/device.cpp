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
#include <memory>

JSClassRef NX::Classes::IO::Device::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.className = "Device";
  def.parentClass = NX::Classes::Emitter::createClass(context);
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
  def.staticValues = NX::Classes::IO::SourceDevice::Properties;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::PushSourceDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::SourceDevice::createClass (context);
  def.className = "PushSourceDevice";
  def.staticFunctions = NX::Classes::IO::PushSourceDevice::Methods;
  def.staticValues = NX::Classes::IO::PushSourceDevice::Properties;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::PullSourceDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::SourceDevice::createClass (context);
  def.className = "PullSourceDevice";
  def.staticFunctions = NX::Classes::IO::PullSourceDevice::Methods;
  def.staticValues = NX::Classes::IO::PullSourceDevice::Properties;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::SinkDevice::createClass (Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::Device::createClass (context);
  def.className = "SinkDevice";
  def.staticFunctions = NX::Classes::IO::SinkDevice::Methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::BidirectionalPullDevice::createClass (Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::PullSourceDevice::createClass (context);
  def.className = "BidirectionalPullDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::SinkDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[1],
  };
  def.staticFunctions = methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::BidirectionalPushDevice::createClass (Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::PushSourceDevice::createClass (context);
  def.className = "BidirectionalPushDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::SinkDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[1],
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
  def.parentClass = NX::Classes::IO::PullSourceDevice::createClass (context);
  def.className = "SeekableSourceDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::SeekableDevice::Methods[0],
    NX::Classes::IO::SeekableDevice::Methods[1],
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
    nullptr
  };
  def.staticFunctions = methods;
  return context->nexus()->defineOrGetClass (def);
}

JSClassRef NX::Classes::IO::BidirectionalSeekableDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = kJSClassDefinitionEmpty;
  def.parentClass = NX::Classes::IO::SeekableDevice::createClass (context);
  def.className = "BidirectionalSeekableDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::PullSourceDevice::Methods[0],
    NX::Classes::IO::PullSourceDevice::Methods[1],
    NX::Classes::IO::SinkDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[1],
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
  def.parentClass = NX::Classes::IO::DualSeekableDevice::createClass (context);
  def.className = "BidirectionalSeekableDevice";
  static const JSStaticFunction methods[]
  {
    NX::Classes::IO::PullSourceDevice::Methods[0],
    NX::Classes::IO::PullSourceDevice::Methods[1],
    NX::Classes::IO::SinkDevice::Methods[0],
    NX::Classes::IO::SinkDevice::Methods[1],
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
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::SourceDevice::Methods[] {
  { nullptr, nullptr, 0 }
};


JSStaticValue NX::Classes::IO::SourceDevice::Properties[] {
  { "eof", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Classes::IO::SourceDevice * dev = NX::Classes::IO::SourceDevice::FromObject(object);
    return JSValueMakeBoolean(ctx, dev->eof());
  }, nullptr, 0 },
  { "type", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Classes::IO::SourceDevice * dev = NX::Classes::IO::SourceDevice::FromObject(object);
    switch(dev->sourceDeviceType()) {
      case NX::Classes::IO::SourceDevice::SourceType::PullType:
        return JSValueMakeString(ctx, ScopedString("pull"));
      case NX::Classes::IO::SourceDevice::SourceType::PushType:
        return JSValueMakeString(ctx, ScopedString("push"));
      default:
        return JSValueMakeString(ctx, ScopedString("unknown"));
    }
  }, nullptr, 0 },
  { nullptr, nullptr, nullptr, 0 }
};

JSStaticValue NX::Classes::IO::PushSourceDevice::Properties[] {
  { "state", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
    NX::Classes::IO::PushSourceDevice * dev = NX::Classes::IO::PushSourceDevice::FromObject(object);
    switch(dev->state()) {
      case NX::Classes::IO::PushSourceDevice::Paused:
        return JSValueMakeString(ctx, ScopedString("paused"));
      case NX::Classes::IO::PushSourceDevice::Resumed:
        return JSValueMakeString(ctx, ScopedString("resumed"));
      default:
        return JSValueMakeString(ctx, ScopedString("unknown"));
    }
  }, nullptr, 0 },
  { nullptr, nullptr, nullptr, 0 }
};


JSStaticFunction NX::Classes::IO::PushSourceDevice::Methods[] {
  { "reset", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::PushSourceDevice * dev = NX::Classes::IO::PushSourceDevice::FromObject(thisObject);
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        return dev->reset(ctx, thisObject);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "pause", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::PushSourceDevice * dev = NX::Classes::IO::PushSourceDevice::FromObject(thisObject);
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        return dev->pause(ctx, thisObject);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "resume", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::PushSourceDevice * dev = NX::Classes::IO::PushSourceDevice::FromObject(thisObject);
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        return dev->resume(ctx, thisObject);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::PullSourceDevice::Methods[] {
  { "read", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      std::size_t length = 0;
      std::size_t bufferSize = 1024 * 1024;
      if (argumentCount > 0) {
        if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
          NX::Value message(ctx, "bad value for length argument");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
          return JSValueMakeUndefined(ctx);
        }
        length = JSValueToNumber(ctx, arguments[0], exception);
      }
      if (argumentCount > 1) {
        if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
          NX::Value message(ctx, "bad value for bufferSize argument");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
          return JSValueMakeUndefined(ctx);
        }
        bufferSize = JSValueToNumber(ctx, arguments[1], exception);
      }
      NX::Classes::IO::PullSourceDevice * dev = NX::Classes::IO::PullSourceDevice::FromObject(thisObject);
      if (!dev) {
        NX::Value message(ctx, "PushSourceDevice object does not implement read()");
        *exception = message.value();
        return JSValueMakeUndefined(ctx);
      }
      JSValueProtect(context->toJSContext(), thisObject);
      NX::Scheduler * scheduler = context->nexus()->scheduler();
      std::size_t chunkSize = bufferSize;
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
        std::cout << "coroutine!\n";
        scheduler->scheduleCoroutine([=]() {
          std::size_t readLength = length;
          try {
            if (readLength == 0) {
              if (auto seekable = dynamic_cast<NX::Classes::IO::SeekableSourceDevice*>(dev))
                readLength = seekable->deviceBytesAvailable();
              if (readLength == 0)
                throw std::runtime_error("must supply read length for non-seekable device");
            }
            char * buffer = (char *)std::malloc(readLength);
            std::size_t readSoFar = 0;
            if(!dev->deviceReady()) {
              scheduler->scheduleTask([=]() {
                NX::Value message(context->toJSContext(), "device not ready");
                return reject(message.value());
              });
              return;
            }
            for(std::size_t i = 0; i < readLength; i += chunkSize)
            {
              std::size_t read = dev->deviceRead(buffer + i, std::min(chunkSize, readLength - i));
              readSoFar += read;
              if (!read || dev->eof()) break;
              scheduler->yield();
            }
            if (readSoFar != readLength)
              buffer = (char *)std::realloc(buffer, readSoFar);
            scheduler->scheduleTask([=]() {
              JSValueRef exp = nullptr;
              JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(), buffer, readSoFar,
                [](void* bytes, void* deallocatorContext) {
                  std::free(bytes);
                }, nullptr, &exp);
              if (exp)
              {
                reject(exp);
              } else {
                resolve(arrayBuffer);
              }
              JSValueUnprotect(context->toJSContext(), thisObject);
            });
          } catch (const std::exception & e) {
            scheduler->scheduleTask([=]() {
              JSValueRef exp = nullptr;
              JSWrapException(context->toJSContext(), e, &exp);
              reject(exp);
              JSValueUnprotect(context->toJSContext(), thisObject);
            });
          }
        });
      });
    }, 0
  },
  { "readSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::IO::PullSourceDevice * dev = NX::Classes::IO::PullSourceDevice::FromObject(thisObject);
        if (argumentCount == 0) {
          throw std::runtime_error("must supply length to read");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
            throw std::runtime_error("bad value for length argument");
          }
        }
        std::size_t length = NX::Value(ctx, arguments[0]).toNumber();
        char * buffer = (char * )std::malloc(length);
        std::size_t readSoFar = 0;
        if(!dev->deviceReady())
          throw std::runtime_error("device not ready");
        readSoFar = dev->deviceRead(buffer, length);
        if (readSoFar != length)
          buffer = (char *)std::realloc(buffer, readSoFar);
        JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, buffer, readSoFar,
                [](void* bytes, void* deallocatorContext) { std::free(bytes); }, nullptr, exception);
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
      std::size_t offset = 0, length = 0;
      NX::Classes::IO::SinkDevice * dev = nullptr;
      try {
        if (argumentCount == 0) {
          throw std::runtime_error("must supply buffer to write");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
            throw std::runtime_error("argument must be TypedArray or ArrayBuffer");
          JSValueRef except = nullptr;
          NX::Object obj(ctx, arguments[0]);
          if (length = JSObjectGetArrayBufferByteLength(ctx, obj.value(), &except))
            arrayBuffer = obj.value();
          else {
            except = nullptr;
            arrayBuffer = JSObjectGetTypedArrayBuffer(ctx, obj.value(), &except);
            if (except) {
              throw std::runtime_error("argument must be TypedArray or ArrayBuffer");
            }
            offset = JSObjectGetTypedArrayByteOffset(ctx, obj, &except);
            length = JSObjectGetTypedArrayByteLength(ctx, obj, &except);
          }
          dev = NX::Classes::IO::SinkDevice::FromObject(thisObject);
          if (!dev) {
            throw std::runtime_error("SinkDevice does not implement write()");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      JSValueProtect(context->toJSContext(), arrayBuffer);
      JSValueProtect(context->toJSContext(), thisObject);
      NX::Scheduler * scheduler = context->nexus()->scheduler();
      std::size_t chunkSize = dev->recommendedWriteBufferSize();
      const char * buffer = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, nullptr);
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
        auto handler = [=](auto handler, std::size_t i, std::size_t length) {
          try {
            if(!dev->deviceReady())
              throw std::runtime_error("device not ready");
            if(i < length)
            {
              dev->deviceWrite(buffer + offset + i, std::min(chunkSize, length - i));
            } else {
              resolve(thisObject);
              JSValueUnprotect(context->toJSContext(), thisObject);
              JSValueUnprotect(context->toJSContext(), arrayBuffer);
              return;
            }
          } catch (const std::exception & e) {
            reject(NX::Object(context->toJSContext(), e));
            JSValueUnprotect(context->toJSContext(), thisObject);
            JSValueUnprotect(context->toJSContext(), arrayBuffer);
            return;
          }
          scheduler->scheduleTask(std::bind(handler, handler, i + std::min(chunkSize, length - i), length));
        };
        scheduler->scheduleTask(std::bind(handler, handler, 0, length));
      });
    }, 0
  },
  { "writeSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      try {
        std::size_t offset = 0, length = 0;
        if (argumentCount == 0) {
          throw std::runtime_error("must supply buffer to write");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
            throw std::runtime_error("bad value for buffer argument");
          JSValueRef except = nullptr;
          NX::Object obj(ctx, arguments[0]);
          length = JSObjectGetArrayBufferByteLength(ctx, obj.value(), &except);
          if (!except)
            arrayBuffer = obj.value();
          else {
            except = nullptr;
            arrayBuffer = JSObjectGetTypedArrayBuffer(ctx, obj.value(), &except);
            if (except) {
              throw std::runtime_error("argument must be TypedArray or ArrayBuffer");
            }
            offset = JSObjectGetTypedArrayByteOffset(ctx, obj.value(), &except);
            length = JSObjectGetTypedArrayByteLength(ctx, obj.value(), &except);
          }
        }
        NX::Classes::IO::SinkDevice * dev = NX::Classes::IO::SinkDevice::FromObject(thisObject);
        const char * buffer = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, exception);
        if(!dev->deviceReady())
          throw std::runtime_error("device not ready");
        dev->deviceWrite(buffer + offset, length);
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
            throw std::runtime_error("position argument must be one of ['begin','current','end']");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      std::size_t offset = NX::Value (ctx, arguments[0]).toNumber();
      std::string position = NX::Value (ctx, arguments[1]).toString();
      NX::Classes::IO::SeekableDevice * dev = NX::Classes::IO::SeekableDevice::FromObject(thisObject);
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
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
          return resolve(NX::Value(context->toJSContext(), newOffset).value());
        } catch (const std::exception & e) {
          return reject(NX::Object(context->toJSContext(), e).value());
        }
      });
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
            throw std::runtime_error("position argument must be one of ['begin','current','end']");
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
            throw std::runtime_error("position argument must be one of ['begin','current','end']");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      std::size_t offset = NX::Value (ctx, arguments[0]).toNumber();
      std::string position = NX::Value (ctx, arguments[1]).toString();
      NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
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
          std::size_t newOffset = dev->deviceReadSeek(offset, pos);
          return resolve(NX::Value(context->toJSContext(), newOffset).value());
        } catch (const std::exception & e) {
          return reject(NX::Object(context->toJSContext(), e).value());
        }
      });
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
            throw std::runtime_error("position argument must be one of ['begin','current','end']");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      std::size_t offset = NX::Value (ctx, arguments[0]).toNumber();
      std::string position = NX::Value (ctx, arguments[1]).toString();
      NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
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
          std::size_t newOffset = dev->deviceWriteSeek(offset, pos);
          return resolve(NX::Value(context->toJSContext(), newOffset).value());
        } catch (const std::exception & e) {
          return reject(NX::Object(context->toJSContext(), e).value());
        }
      });
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
            throw std::runtime_error("position argument must be one of ['begin','current','end']");
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
            throw std::runtime_error("position argument must be one of ['begin','current','end']");
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
