#include "context.h"
#include "classes/stream.h"

JSClassRef NX::Classes::Stream::createClass (NX::Context * context)
{
  return context->defineOrGetClass(NX::Classes::Stream::Class);
}

const JSClassDefinition NX::Classes::Stream::Class {
  0, kJSClassAttributeNone, "Stream", nullptr, NX::Classes::Stream::Properties,
  NX::Classes::Stream::Methods, nullptr, NX::Classes::Stream::Finalize
};

const JSStaticValue NX::Classes::Stream::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

#include <iostream>

const JSStaticFunction NX::Classes::Stream::Methods[] {
  { "readAsBuffer", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Classes::Stream * stream = NX::Classes::Stream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid Stream instance");
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
      NX::Classes::Stream * stream = NX::Classes::Stream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid Stream instance");
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
      NX::Classes::Stream * stream = NX::Classes::Stream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid Stream instance");
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
      NX::Classes::Stream * stream = NX::Classes::Stream::FromObject(thisObject);
      if (!stream) {
        NX::Value message(ctx, "invalid Stream instance");
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
