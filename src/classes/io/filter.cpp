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
#include "classes/io/filter.h"

JSClassRef NX::Classes::IO::Filter::createClass (NX::Context * context)
{
  return context->defineOrGetClass(NX::Classes::IO::Filter::Class);
}

JSClassDefinition NX::Classes::IO::Filter::Class {
  0, kJSClassAttributeNone, "Filter", nullptr, NX::Classes::IO::Filter::Properties,
  NX::Classes::IO::Filter::Methods, nullptr, NX::Classes::IO::Filter::Finalize
};

JSStaticValue NX::Classes::IO::Filter::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

JSStaticFunction NX::Classes::IO::Filter::Methods[] {
  { "process", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      try {
        if (argumentCount == 0) {
          throw std::runtime_error("must supply buffer to process");
        } else {
          if (!JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
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
        NX::Classes::IO::Filter * filter = NX::Classes::IO::Filter::FromObject(thisObject);
        JSValueProtect(context->toJSContext(), thisObject);
        for(int i = 0; i < argumentCount; i++)
          JSValueProtect(context->toJSContext(), arguments[i]);
        boost::shared_ptr<NX::Scheduler> scheduler = context->nexus()->scheduler();
        std::size_t chunkSize = 4096;
        JSObjectRef arrayBuffer= NX::Object(ctx, arguments[0]).value();
        scheduler->scheduleCoroutine([=]() {
          JSContextRef ctx = context->toJSContext();
          try {
            const char * buffer = (const char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, nullptr);
            std::size_t length = JSObjectGetArrayBufferByteLength(ctx, arrayBuffer, nullptr);
            std::size_t outLengthEstimatedTotal = filter->processBuffer(buffer, length);
            std::size_t outPos = 0;
            char * newBuffer = (char *)malloc(outLengthEstimatedTotal);
            for(std::size_t i = 0; i < length; i += chunkSize)
            {
              outPos += filter->processBuffer(buffer + i, std::min(chunkSize, length - i), newBuffer + outPos, outLengthEstimatedTotal - outPos);
              scheduler->yield();
            }
            if (outPos != outLengthEstimatedTotal)
              newBuffer = (char *)realloc(newBuffer, outPos);
            JSObjectRef outputArrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, newBuffer, outPos,
              [](void* bytes, void* deallocatorContext) { free(bytes); }, nullptr, exception);
            scheduler->scheduleTask([=]() {
              JSValueRef args[] { outputArrayBuffer };
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
      return NX::Globals::Promise::createPromise(ctx, executor, exception);
    }, 0
  },
  { "processSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      try {
        NX::Classes::IO::Filter * filter = NX::Classes::IO::Filter::FromObject(thisObject);
        JSObjectRef arrayBuffer = nullptr;
        if (argumentCount == 0) {
          throw std::runtime_error("must supply buffer to write");
        } else {
          if (!JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
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
        const char * buffer =  (const char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, exception);
        std::size_t length = JSObjectGetArrayBufferByteLength(ctx, arrayBuffer, exception);
        std::size_t estimatedOutLength = filter->processBuffer(buffer, length);
        char * newBuffer = (char *)malloc(estimatedOutLength);
        std::size_t outLength = filter->processBuffer(buffer, length, newBuffer, estimatedOutLength);
        if (outLength != estimatedOutLength)
          newBuffer = (char *)realloc(newBuffer, outLength);
        JSObjectRef output = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, newBuffer, outLength,
          [](void* bytes, void* deallocatorContext) { free(bytes); }, nullptr, exception);
        return output;
      } catch( const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};
