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

#include <memory>

JSClassRef NX::Classes::IO::Filter::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::Filter::Class;
  def.parentClass = NX::Classes::Base::createClass(context);
  return context->nexus()->defineOrGetClass(def);
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
    size_t argumentCount, const JSValueRef originalArguments[], JSValueRef* exception) -> JSValueRef
    {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      std::size_t length = 0, offset = 0;
      try {
        if (argumentCount == 0) {
          throw NX::Exception("must supply buffer to write");
        } else {
          auto type = JSValueGetType(ctx, originalArguments[0]);
          if (type != kJSTypeObject && type != kJSTypeNull)
            throw NX::Exception("bad value for buffer argument");
          else if (type != kJSTypeNull) {
            JSValueRef except = nullptr;
            NX::Object obj(ctx, originalArguments[0]);
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
        }
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      const char * buffer = arrayBuffer ? (char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, nullptr) + offset : nullptr;
      JSValueProtect(context->toJSContext(), thisObject);
      if (arrayBuffer)
        JSValueProtect(context->toJSContext(), arrayBuffer);
      auto executor = [=](JSContextRef ctx, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
        NX::Context *context = NX::Context::FromJsContext(ctx);
        NX::Scheduler *scheduler = context->nexus()->scheduler();
        NX::Classes::IO::Filter *filter = NX::Classes::IO::Filter::FromObject(thisObject);
        if (!filter) {
          if (arrayBuffer)
            JSValueUnprotect(context->toJSContext(), arrayBuffer);
          JSValueUnprotect(context->toJSContext(), thisObject);
          return reject(ctx, NX::Exception("filter object does not implement process()").toError(ctx));
        }
        auto handler = [=](auto handler, const char *inBuffer, std::size_t inLength,
                           char *outBuffer, std::size_t outLength,
                           char *outPtr, std::size_t outRemaining)
        {
          try {
            if (auto sizeNeeded = filter->processBuffer(&inBuffer, &inLength, &outPtr, &outRemaining)) {
              outLength += sizeNeeded;
              auto remappedOutOffset = outPtr - outBuffer;
              outBuffer = static_cast<char *>(WTF::fastRealloc(outBuffer, outLength));
              outPtr = outBuffer + remappedOutOffset;
              context->nexus()->scheduler()->scheduleTask(
                std::bind<void>(handler, handler, inBuffer, inLength, outBuffer, outLength, outPtr, outRemaining)
              );
              return;
            }
            if (outRemaining > 0)
              outBuffer = static_cast<char *>(WTF::fastRealloc(outBuffer, outLength - outRemaining));
            JSValueRef exp = nullptr;
            JSObjectRef outputArrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(),
                                                                                   outBuffer, outLength,
                                                                                   [](void *bytes,
                                                                                      void *deallocatorContext) {
                                                                                     WTF::fastFree(bytes);
                                                                                   }, nullptr, &exp);
            if (!exp)
              resolve(context->toJSContext(), outputArrayBuffer);
            else
              reject(context->toJSContext(), exp);
          } catch(const std::exception & e) {
            reject(context->toJSContext(), NX::Object(context->toJSContext(), e));
          }
          JSValueUnprotect(context->toJSContext(), thisObject);
          if (arrayBuffer)
            JSValueUnprotect(context->toJSContext(), arrayBuffer);
        };
        std::size_t originalEstimate = filter->estimateOutputLength(buffer, length);
        auto outBuffer = static_cast<char *>(WTF::fastMalloc(originalEstimate));
        scheduler->scheduleTask(std::bind(handler, handler, buffer, length,
                                                    outBuffer, originalEstimate, outBuffer, originalEstimate));
      };
      return NX::Globals::Promise::createPromise(ctx, executor);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
