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
    size_t argumentCount, const JSValueRef originalArguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      JSObjectRef arrayBuffer = nullptr;
      std::size_t length = 0, offset = 0;
      try {
        if (argumentCount == 0) {
          throw NX::Exception("must supply buffer to write");
        } else {
          if (JSValueGetType(ctx, originalArguments[0]) != kJSTypeObject)
            throw NX::Exception("bad value for buffer argument");
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
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      char * buffer = (char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, nullptr);
      JSValueProtect(context->toJSContext(), thisObject);
      JSValueProtect(context->toJSContext(), arrayBuffer);
      return NX::Globals::Promise::createPromise(ctx,
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
          NX::Scheduler* scheduler = context->nexus()->scheduler();
          NX::Classes::IO::Filter * filter = NX::Classes::IO::Filter::FromObject(thisObject);
          if (!filter) {
            JSValueUnprotect(context->toJSContext(), arrayBuffer);
            NX::Object exp(context->toJSContext(), NX::Exception("filter object does not implement process()"));
            return reject(exp);
          }
          const std::size_t chunkSize = UINT16_MAX; // TODO: MAKE THIS ADJUSTABLE
          auto handler = [=](auto handler, std::size_t i, char * newBuffer, std::size_t outPos, std::size_t outLengthEstimatedTotal) {
            if (i < length) {
              try {
                if (std::size_t out = filter->processBuffer(buffer + offset + i, std::min(chunkSize, length - i),
                  newBuffer + outPos, outLengthEstimatedTotal))
                {
                  outLengthEstimatedTotal -= out;
                  outPos += out;
                } else {
                  std::size_t newEstimate = filter->estimateOutputLength(buffer + offset + i, length - i);
                  newBuffer = (char *)std::realloc(newBuffer, outPos + newEstimate);
                  outLengthEstimatedTotal += newEstimate;
                  i -= chunkSize;
                }
                scheduler->scheduleTask(std::bind(handler, handler, i + chunkSize, newBuffer, outPos, outLengthEstimatedTotal));
              } catch(const std::exception & e) {
                reject(NX::Object(context->toJSContext(), e));
                return;
              }
            } else {
              JSObjectRef outputArrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(), newBuffer, outPos,
                [](void* bytes, void* deallocatorContext) { std::free(bytes); }, nullptr, exception);
              JSValueUnprotect(context->toJSContext(), thisObject);
              JSValueUnprotect(context->toJSContext(), arrayBuffer);
              return resolve(outputArrayBuffer);
            }
          };
          std::size_t outLengthEstimatedTotal = filter->estimateOutputLength(buffer, length);
          scheduler->scheduleTask(std::bind(handler, handler, 0, (char*)std::malloc(outLengthEstimatedTotal), 0, outLengthEstimatedTotal));
      });
    }, 0
  },
  { "processSync", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = NX::Context::FromJsContext(ctx);
      char * newBuffer = nullptr;
      try {
        NX::Classes::IO::Filter * filter = NX::Classes::IO::Filter::FromObject(thisObject);
        if (!filter) {
          throw NX::Exception("filter object does not implement processSync()");
        }
        JSObjectRef arrayBuffer = nullptr;
        if (argumentCount == 0) {
          throw NX::Exception("must supply buffer to write");
        } else {
          if (JSValueGetType(ctx, arguments[0]) != kJSTypeObject)
            throw NX::Exception("bad value for buffer argument");
          JSValueRef except = nullptr;
          NX::Object obj(ctx, arguments[0]);
          std::size_t length = JSObjectGetArrayBufferByteLength(ctx, obj.value(), &except);
          if (!except)
            arrayBuffer = obj.value();
          else {
            except = nullptr;
            arrayBuffer = JSObjectGetTypedArrayBuffer(ctx, obj.value(), &except);
            if (except) {
              throw NX::Exception("argument must be TypedArray or ArrayBuffer");
            }
          }
        }
        char * buffer =  (char *)JSObjectGetArrayBufferBytesPtr(ctx, arrayBuffer, exception);
        std::size_t length = JSObjectGetArrayBufferByteLength(ctx, arrayBuffer, exception);
        std::size_t estimatedOutLength = filter->estimateOutputLength(buffer, length);
        newBuffer = (char *)std::malloc(estimatedOutLength);
        std::size_t outLength = filter->processBuffer(buffer, length, newBuffer, estimatedOutLength);
        if (outLength == 0)
        {
          throw NX::Exception("insufficient memory for filter processing");
        }
        if (outLength != estimatedOutLength)
          newBuffer = (char *)std::realloc(newBuffer, outLength);
        JSObjectRef output = JSObjectMakeArrayBufferWithBytesNoCopy(ctx, newBuffer, outLength,
          [](void* bytes, void* deallocatorContext) { std::free(bytes); }, nullptr, exception);
        return output;
      } catch( const std::exception & e) {
        if (newBuffer)
          std::free(newBuffer);
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};
