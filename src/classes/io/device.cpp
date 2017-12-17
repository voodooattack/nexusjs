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
    nullptr
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
    nullptr
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
    nullptr
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
    nullptr
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
    nullptr
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
  { "close", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef
    {
      NX::Classes::IO::Device * dev = nullptr;
      try {
        dev = NX::Classes::IO::Device::FromObject(thisObject);
        if (!dev) {
          return NX::Globals::Promise::reject(ctx, NX::Exception("Device does not implement close()").toError(ctx));
        }
        dev->deviceClose();
        return NX::Globals::Promise::resolve(ctx, thisObject);
      } catch(const std::exception & e) {
        return NX::Globals::Promise::reject(ctx, NX::Object(ctx, e));
      }
    }, 0
  },
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
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        return dev->reset(ctx, thisObject);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
  }, 0
  },
  { "pause", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::PushSourceDevice * dev = NX::Classes::IO::PushSourceDevice::FromObject(thisObject);
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        return dev->pause(ctx, thisObject);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "resume", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Classes::IO::PushSourceDevice * dev = NX::Classes::IO::PushSourceDevice::FromObject(thisObject);
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        return dev->resume(ctx, thisObject);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::PullSourceDevice::Methods[] {
  { "read", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      std::size_t length = 0;
      if (argumentCount > 0) {
        if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
          NX::Value message(ctx, "bad value for length argument");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
          return JSValueMakeUndefined(ctx);
        }
        length = static_cast<size_t>(JSValueToNumber(ctx, arguments[0], exception));
      }
      NX::Classes::IO::PullSourceDevice * dev = NX::Classes::IO::PullSourceDevice::FromObject(thisObject);
      if (!dev) {
        NX::Value message(ctx, "PushSourceDevice object does not implement read()");
        *exception = message.value();
        return JSValueMakeUndefined(ctx);
      }
      JSValueProtect(context->toJSContext(), thisObject);
      NX::Scheduler * scheduler = context->nexus()->scheduler();
      return NX::Globals::Promise::createPromise(ctx,
        [=](JSContextRef ctx, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        auto readHandler = [=](auto readHandler, std::size_t readLength) {
          char * buffer = nullptr;
          try {
            if (readLength == 0) {
              if (auto seekable = dynamic_cast<NX::Classes::IO::SeekableSourceDevice*>(dev))
                readLength = seekable->deviceBytesAvailable();
              if (readLength == 0)
                throw NX::Exception("must supply read length for non-seekable device");
            }
            buffer = (char *)WTF::fastMalloc(readLength);
            if(!dev->deviceReady()) {
              return reject(context->toJSContext(), NX::Exception("device not ready").toError(context->toJSContext()));
            }
            std::size_t readSoFar = dev->deviceRead(buffer, readLength);
            if (readSoFar < readLength)
              buffer = (char *)WTF::fastRealloc(buffer, readSoFar);
            JSValueRef exp = nullptr;
            JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(), buffer, readSoFar,
                                                                             [](void* bytes, void* deallocatorContext) {
                                                                               WTF::fastFree(bytes);
                                                                             }, nullptr, &exp);
            if (exp)
            {
              reject(context->toJSContext(), exp);
            } else {
              resolve(context->toJSContext(), arrayBuffer);
            }
            JSValueUnprotect(context->toJSContext(), thisObject);
          } catch (const std::exception & e) {
            if (buffer)
              WTF::fastFree(buffer);
            JSValueRef exp = nullptr;
            JSWrapException(context->toJSContext(), e, &exp);
            reject(context->toJSContext(), exp);
            JSValueUnprotect(context->toJSContext(), thisObject);
          }
        };
        scheduler->scheduleTask(std::bind(readHandler, readHandler, length));
      });
    }, 0
  },
  { "readSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      char * buffer = nullptr;
      try {
//        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::IO::PullSourceDevice * dev = NX::Classes::IO::PullSourceDevice::FromObject(thisObject);
        if (argumentCount == 0) {
          throw NX::Exception("must supply length to read");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber) {
            throw NX::Exception("bad value for length argument");
          }
        }
        std::size_t length = static_cast<size_t>(NX::Value(ctx, arguments[0]).toNumber());
        buffer = (char * )WTF::fastMalloc(length);
        std::size_t readSoFar = 0;
        if(!dev->deviceReady())
          throw NX::Exception("device not ready");
        readSoFar = dev->deviceRead(buffer, length);
        if (readSoFar != length)
          buffer = (char *)WTF::fastRealloc(buffer, readSoFar);
        JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, buffer, readSoFar,
                [](void* bytes, void* deallocatorContext) { WTF::fastFree(bytes); }, nullptr, exception);
        return arrayBuffer;
      } catch(const std::exception & e) {
        if (buffer)
          WTF::fastFree(buffer);
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::SinkDevice::Methods[] {
  { "write", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
      size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef
    {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      std::size_t offset = 0, length = 0;
      NX::Classes::IO::SinkDevice * dev = nullptr;
      try {
        dev = NX::Classes::IO::SinkDevice::FromObject(thisObject);
        if (!dev) {
          return NX::Globals::Promise::reject(ctx, NX::Exception("SinkDevice does not implement write()").toError(ctx));
        }
        if (argumentCount == 0) {
          return NX::Globals::Promise::reject(ctx, NX::Exception("must supply buffer to write").toError(ctx));
        } else {
          auto type = JSValueGetType(ctx, arguments[0]);
          if (type != kJSTypeNull) {
            if (type != kJSTypeObject) {
              return NX::Globals::Promise::reject(ctx,
                                                  NX::Exception("argument must be TypedArray, ArrayBuffer, or null")
                                                    .toError(ctx));
            }
            JSValueRef except = nullptr;
            JSObjectRef typedArrayOrArrayBuffer = JSValueToObject(ctx, arguments[0], &except);
            if (except)
              return NX::Globals::Promise::reject(ctx, except);
            length = JSObjectGetArrayBufferByteLength(ctx, typedArrayOrArrayBuffer, &except);
            if (!except) {
              arrayBuffer = typedArrayOrArrayBuffer;
            }
            else
            {
              except = nullptr;
              arrayBuffer = JSObjectGetTypedArrayBuffer(ctx, typedArrayOrArrayBuffer, &except);
              if (except) {
                return NX::Globals::Promise::reject(ctx, except);
              }
              offset = JSObjectGetTypedArrayByteOffset(ctx, typedArrayOrArrayBuffer, &except);
              length = JSObjectGetTypedArrayByteLength(ctx, typedArrayOrArrayBuffer, &except);
            }
          }
        }
      } catch(const std::exception & e) {
        return NX::Globals::Promise::reject(ctx, NX::Object(ctx, e));
      }
      if (!arrayBuffer) {
        if (dev->deviceReady()) {
          try {
            dev->deviceWrite(nullptr, 0);
          } catch (const std::exception &e) {}
        }
        return NX::Globals::Promise::resolve(ctx, thisObject);
      }
      if (!length)
        return NX::Globals::Promise::resolve(ctx, thisObject);
      JSValueProtect(context->toJSContext(), thisObject);
      JSValueProtect(context->toJSContext(), arrayBuffer);
      JSValueRef exp = nullptr;
      auto buffer = static_cast<char *>(
                      JSObjectGetArrayBufferBytesPtr(context->toJSContext(), arrayBuffer, &exp)) + offset;
      if (exp)
        return NX::Globals::Promise::reject(ctx, exp);
      NX::Scheduler * scheduler = context->nexus()->scheduler();
      return NX::Globals::Promise::createPromise(ctx,
        [=](JSContextRef ctx, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        auto writeHandler = [=](auto writeHandler, std::size_t written) {
          try {
            while (written < length) {
              if (!dev->deviceReady() && dev->deviceOpen()) {
                if (auto ec = dev->deviceError()) {
                  throw NX::Exception(ec);
                }
                scheduler->scheduleTask(std::bind<void>(writeHandler, writeHandler, written));
                return;
              }
              if (auto ec = dev->deviceError()) {
                throw NX::Exception(ec);
              }
              else if (!dev->deviceOpen())
                break;
              auto max = dev->maxWriteBufferSize();
              if (auto size = std::min(dev->recommendedWriteBufferSize(), std::size_t(length - written))) {
                if (size > max) size = max;
                written += dev->deviceWrite(buffer + written, size);
                scheduler->scheduleTask(std::bind<void>(writeHandler, writeHandler, written));
                return;
              } else
                break;
            }
          } catch (const std::exception & e) {
            reject(context->toJSContext(), NX::Object(context->toJSContext(), e));
            JSValueUnprotect(context->toJSContext(), arrayBuffer);
            JSValueUnprotect(context->toJSContext(), thisObject);
            return;
          }
          resolve(context->toJSContext(), JSValueMakeNumber(context->toJSContext(), written));
          JSValueUnprotect(context->toJSContext(), arrayBuffer);
          JSValueUnprotect(context->toJSContext(), thisObject);
        };
        scheduler->scheduleTask(std::bind(writeHandler, writeHandler, 0));
      });
    }, 0
  },
  { "writeSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      try {
        std::size_t offset = 0, length = 0;
        if (argumentCount == 0) {
          throw NX::Exception("must supply buffer to write");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
            throw NX::Exception("bad value for buffer argument");
          JSValueRef except = nullptr;
          NX::Object obj(ctx, arguments[0]);
          length = JSObjectGetArrayBufferByteLength(ctx, obj.value(), &except);
          if (!except)
            arrayBuffer = obj.value();
          else {
            except = nullptr;
            arrayBuffer = JSObjectGetTypedArrayBuffer(ctx, obj.value(), &except);
            if (except) {
              throw NX::Exception("argument must be TypedArray or ArrayBuffer");
            }
            offset = JSObjectGetTypedArrayByteOffset(ctx, obj.value(), &except);
            length = JSObjectGetTypedArrayByteLength(ctx, obj.value(), &except);
          }
        }
        NX::Classes::IO::SinkDevice * dev = NX::Classes::IO::SinkDevice::FromObject(thisObject);
        auto * buffer = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, exception);
        if(!dev->deviceReady())
          throw NX::Exception("device not ready");
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
          throw NX::Exception("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw NX::Exception("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw NX::Exception("value for position argument is not a string");
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw NX::Exception("position argument must be one of ['begin','current','end']");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      std::size_t offset = static_cast<size_t>(NX::Value (ctx, arguments[0]).toNumber());
      std::string position = NX::Value (ctx, arguments[1]).toString();
      NX::Classes::IO::SeekableDevice * dev = NX::Classes::IO::SeekableDevice::FromObject(thisObject);
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](JSContextRef ctx, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        try {
          Device::Position pos = Beginning;
          if (boost::iequals(position, "begin"))
            pos = Beginning;
          else if (boost::iequals(position, "current"))
            pos = Current;
          else if (boost::iequals(position, "end"))
            pos = End;
          if(!dev->deviceReady())
            throw NX::Exception("device not ready");
          std::size_t newOffset = dev->deviceSeek(offset, pos);
          return resolve(ctx, NX::Value(context->toJSContext(), newOffset).value());
        } catch (const std::exception & e) {
          return reject(ctx, NX::Object(context->toJSContext(), e).value());
        }
      });
    }, 0
  },
  { "seekSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw NX::Exception("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw NX::Exception("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw NX::Exception("value for position argument is not a string");
          NX::Value position(ctx, arguments[0]);
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw NX::Exception("position argument must be one of ['begin','current','end']");
          }
          NX::Classes::IO::SeekableDevice * dev = NX::Classes::IO::SeekableDevice::FromObject(thisObject);
          Device::Position pos = Beginning;
          if (boost::iequals(position.toString(), "begin"))
            pos = Beginning;
          else if (boost::iequals(position.toString(), "current"))
            pos = Current;
          else if (boost::iequals(position.toString(), "end"))
            pos = End;
          if (!dev->deviceReady())
            throw NX::Exception("device not ready");
          std::size_t newOffset = dev->deviceSeek(static_cast<size_t>(offset.toNumber()), pos);
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
          throw NX::Exception("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw NX::Exception("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw NX::Exception("value for position argument is not a string");
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw NX::Exception("position argument must be one of ['begin','current','end']");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      std::size_t offset = static_cast<size_t>(NX::Value (ctx, arguments[0]).toNumber());
      std::string position = NX::Value (ctx, arguments[1]).toString();
      NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](JSContextRef ctx, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
        try {
          Device::Position pos = Beginning;
          if (boost::iequals(position, "begin"))
            pos = Beginning;
          else if (boost::iequals(position, "current"))
            pos = Current;
          else if (boost::iequals(position, "end"))
            pos = End;
          if(!dev->deviceReady())
            throw NX::Exception("device not ready");
          std::size_t newOffset = dev->deviceReadSeek(offset, pos);
          return resolve(ctx, NX::Value(context->toJSContext(), newOffset).value());
        } catch (const std::exception & e) {
          return reject(ctx, NX::Object(context->toJSContext(), e).value());
        }
      });
    }, 0
  },
  { "writeSeek", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw NX::Exception("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw NX::Exception("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw NX::Exception("value for position argument is not a string");
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw NX::Exception("position argument must be one of ['begin','current','end']");
          }
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      std::size_t offset = static_cast<size_t>(NX::Value (ctx, arguments[0]).toNumber());
      std::string position = NX::Value (ctx, arguments[1]).toString();
      NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
      return NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](JSContextRef ctx, ResolveRejectHandler resolve, ResolveRejectHandler reject)
      {
        try {
          Device::Position pos = Beginning;
          if (boost::iequals(position, "begin"))
            pos = Beginning;
          else if (boost::iequals(position, "current"))
            pos = Current;
          else if (boost::iequals(position, "end"))
            pos = End;
          if(!dev->deviceReady())
            throw NX::Exception("device not ready");
          std::size_t newOffset = dev->deviceWriteSeek(offset, pos);
          return resolve(ctx, NX::Value(context->toJSContext(), newOffset).value());
        } catch (const std::exception & e) {
          return reject(ctx, NX::Object(context->toJSContext(), e).value());
        }
      });
    }, 0
  },
  { "readSeekSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw NX::Exception("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw NX::Exception("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw NX::Exception("value for position argument is not a string");
          NX::Value position(ctx, arguments[0]);
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw NX::Exception("position argument must be one of ['begin','current','end']");
          }
          NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
          Device::Position pos = Beginning;
          if (boost::iequals(position.toString(), "begin"))
            pos = Beginning;
          else if (boost::iequals(position.toString(), "current"))
            pos = Current;
          else if (boost::iequals(position.toString(), "end"))
            pos = End;
          if(!dev->deviceReady())
            throw NX::Exception("device not ready");
          std::size_t newOffset = dev->deviceReadSeek(static_cast<size_t>(offset.toNumber()), pos);
          return NX::Value(ctx, newOffset).value();
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "writeSeekSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        if (argumentCount != 2) {
          throw NX::Exception("must supply stream offset and position");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeNumber)
            throw NX::Exception("value for offset argument is not a number");
          if (JSValueGetType(ctx, arguments[1]) != kJSTypeString)
            throw NX::Exception("value for position argument is not a string");
          NX::Value position(ctx, arguments[0]);
          NX::Value offset(ctx, arguments[1]);
          std::string offStr(offset.toString());
          if (!boost::iequals("begin", offStr) && !boost::iequals("current", offStr) && !boost::iequals("end", offStr)) {
            throw NX::Exception("position argument must be one of ['begin','current','end']");
          }
          NX::Classes::IO::DualSeekableDevice * dev = NX::Classes::IO::DualSeekableDevice::FromObject(thisObject);
          Device::Position pos = Beginning;
          if (boost::iequals(position.toString(), "begin"))
            pos = Beginning;
          else if (boost::iequals(position.toString(), "current"))
            pos = Current;
          else if (boost::iequals(position.toString(), "end"))
            pos = End;
          if(!dev->deviceReady())
            throw NX::Exception("device not ready");
          std::size_t newOffset = dev->deviceWriteSeek(static_cast<size_t>(offset.toNumber()), pos);
          return NX::Value(ctx, newOffset).value();
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};
