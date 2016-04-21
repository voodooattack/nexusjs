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

#include "context.h"
#include "globals/promise.h"
#include "classes/io/stream.h"

JSClassRef NX::Classes::IO::Stream::createClass (NX::Context * context)
{
  return context->defineOrGetClass(NX::Classes::IO::Stream::Class);
}

const JSClassDefinition NX::Classes::IO::Stream::Class {
  0, kJSClassAttributeNone, "Stream", nullptr, NX::Classes::IO::Stream::Properties,
  NX::Classes::IO::Stream::Methods, nullptr, NX::Classes::IO::Stream::Finalize
};

const JSStaticValue NX::Classes::IO::Stream::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::Stream::Methods[] {
  { nullptr, nullptr, 0 }
};

JSClassRef NX::Classes::IO::ReadableStream::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::ReadableStream::Class;
  def.parentClass = NX::Classes::IO::Stream::createClass(context);
  return context->defineOrGetClass(def);
}

JSValueRef NX::Classes::IO::ReadableStream::readAsBuffer (JSContextRef ctx, JSObjectRef thisObject, std::size_t length)
{
  try {
    JSValueRef promise = myDevice["read"]->toObject()->call(myDevice, std::vector<JSValueRef> { NX::Value(ctx, length).value() }, nullptr);
    for(auto & i : myFilters)
      promise = NX::Object(ctx, promise)["then"]->toObject()->call(NX::Object(ctx, promise).value(), std::vector<JSValueRef> {
        NX::Object(ctx, i)["process"]->toObject()->bind(i, 0, nullptr, nullptr)
      });
    return promise;
  } catch(const std::exception & e) {
    return JSWrapException(ctx, e, nullptr);
  }
}

JSValueRef NX::Classes::IO::ReadableStream::readAsBufferSync (JSContextRef ctx, JSObjectRef thisObject,
                                                              std::size_t length, JSValueRef * exception)
{
  try {
    JSValueRef result = myDevice["readSync"]->toObject()->call(myDevice, std::vector<JSValueRef> {
      NX::Value(ctx, length).value()
    }, nullptr);
    for(auto & i : myFilters) {
      result = NX::Object(ctx, i)["processSync"]->toObject()->call(i, std::vector<JSValueRef> { result }, exception);
      if (exception && *exception)
        break;
    }
    return result;
  } catch(const std::exception & e) {
    return JSWrapException(ctx, e, nullptr);
  }
}

JSValueRef NX::Classes::IO::ReadableStream::readAsString (JSContextRef ctx, JSObjectRef thisObject,
                                                          const std::string & encoding, std::size_t length)
{

}

JSValueRef NX::Classes::IO::ReadableStream::readAsStringSync (JSContextRef ctx, JSObjectRef thisObject,
                                                              const std::string & encoding, std::size_t length,
                                                              JSValueRef * exception)
{

}

JSClassRef NX::Classes::IO::WritableStream::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::WritableStream::Class;
  def.parentClass = NX::Classes::IO::Stream::createClass(context);
  return context->defineOrGetClass(def);
}

JSValueRef NX::Classes::IO::WritableStream::writeBuffer (JSContextRef ctx, JSObjectRef thisObject, JSObjectRef buffer)
{

}

JSValueRef NX::Classes::IO::WritableStream::writeBufferSync (JSContextRef ctx, JSObjectRef thisObject,
                                                             JSObjectRef buffer, JSValueRef * exception)
{

}

JSValueRef NX::Classes::IO::WritableStream::writeString (JSContextRef ctx, JSObjectRef thisObject,
                                                         JSObjectRef writable, const std::string & encoding)
{

}

JSValueRef NX::Classes::IO::WritableStream::writeStringSync (JSContextRef ctx, JSObjectRef thisObject,
                                                             JSObjectRef writable, const std::string & encoding, JSValueRef * exception)
{

}


const JSClassDefinition NX::Classes::IO::ReadableStream::Class {
  0, kJSClassAttributeNone, "ReadableStream", nullptr, NX::Classes::IO::ReadableStream::Properties,
  NX::Classes::IO::ReadableStream::Methods, nullptr, NX::Classes::IO::ReadableStream::Finalize
};

const JSStaticValue NX::Classes::IO::ReadableStream::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::ReadableStream::Methods[] {
  { "readAsBuffer", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Classes::IO::ReadableStream * stream = NX::Classes::IO::ReadableStream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid ReadableStream instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      std::size_t length = (std::size_t)-1;
      if (argumentCount >= 1) {
        length = JSValueToNumber(ctx, arguments[0], exception);
        if (exception && *exception)
        {
          return JSValueMakeUndefined(ctx);
        }
      }
      return stream->readAsBuffer(ctx, thisObject, length);
    }, 0
  },
  { "readAsString", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Classes::IO::ReadableStream * stream = NX::Classes::IO::ReadableStream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid ReadableStream instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      std::size_t length = (std::size_t)-1;
      std::string encoding("UTF8");
      try {
        if (argumentCount >= 1) {
          length = NX::Value(ctx, arguments[0]).toNumber();
        }
        if (argumentCount >= 2) {
          encoding = NX::Value(ctx, arguments[1]).toString();
        }
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      return stream->readAsString(ctx, thisObject, encoding, length);
    }, 0
  },
  { "readAsBufferSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Classes::IO::ReadableStream * stream = NX::Classes::IO::ReadableStream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid ReadableStream instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      std::size_t length = (std::size_t)-1;
      try {
        if (argumentCount >= 1) {
          length = NX::Value(ctx, arguments[0]).toNumber();
        }
        return stream->readAsBufferSync(ctx, thisObject, length, exception);
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "readAsStringSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Classes::IO::ReadableStream * stream = NX::Classes::IO::ReadableStream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid ReadableStream instance");
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        return JSValueMakeUndefined(ctx);
      }
      std::size_t length = (std::size_t)-1;
      std::string encoding("UTF8");
      try {
        if (argumentCount >= 1) {
          length = NX::Value(ctx, arguments[0]).toNumber();
        }
        if (argumentCount >= 2) {
          encoding = NX::Value(ctx, arguments[1]).toString();
        }
        return stream->readAsStringSync(ctx, thisObject, encoding, length, exception);
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Classes::IO::WritableStream::Class {
  0, kJSClassAttributeNone, "WritableStream", nullptr, NX::Classes::IO::WritableStream::Properties,
  NX::Classes::IO::WritableStream::Methods, nullptr, NX::Classes::IO::WritableStream::Finalize
};

const JSStaticValue NX::Classes::IO::WritableStream::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::WritableStream::Methods[] {
  { nullptr, nullptr, 0 }
};
