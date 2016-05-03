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

#ifndef CLASSES_IO_STREAM_H
#define CLASSES_IO_STREAM_H

#include <JavaScript.h>

#include "classes/emitter.h"
#include "classes/io/device.h"
#include "classes/io/filter.h"
#include "util.h"
#include "globals/promise.h"

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
  {
    namespace IO
    {
      class Stream: public virtual Emitter
      {
      private:
        static const JSClassDefinition Class;
        static const JSStaticValue Properties[];
        static const JSStaticFunction Methods[];

        static void Finalize(JSObjectRef object) {
        }

      public:

        static NX::Classes::IO::Stream * FromObject(JSObjectRef object) {
          return dynamic_cast<NX::Classes::IO::Stream *>(NX::Classes::Base::FromObject(object));
        }

        static JSClassRef createClass(NX::Context * context);

        virtual ~Stream() {}

      protected:
        Stream() {}
      };

      class ReadableStream: public virtual NX::Classes::IO::Stream {
      private:
        static const JSClassDefinition Class;
        static const JSStaticValue Properties[];
        static const JSStaticFunction Methods[];

        static void Finalize(JSObjectRef object) {
        }

        static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                       const JSValueRef arguments[], JSValueRef* exception)
        {
          NX::Context * context = NX::Context::FromJsContext(ctx);
          JSClassRef _class = context->nexus()->defineOrGetClass(NX::Classes::IO::ReadableStream::Class);
          try {
            if (argumentCount < 1) {
              throw std::runtime_error("missing parameter device in call to ReadableStream constructor");
            }
            return JSObjectMake(ctx, _class, static_cast<NX::Classes::Base*>(new ReadableStream(NX::Object(context->toJSContext(), arguments[0]))));
          } catch(const std::exception & e) {
            JSWrapException(ctx, e, exception);
            return JSObjectMake(ctx, nullptr, nullptr);
          }
        }

      public:
        ReadableStream(const NX::Object & device): myDevice(device) { }
        virtual ~ReadableStream() { }

        static JSClassRef createClass(NX::Context * context);

        static NX::Classes::IO::ReadableStream * FromObject(JSObjectRef object) {
          return dynamic_cast<NX::Classes::IO::ReadableStream *>(NX::Classes::Base::FromObject(object));
        }

        static JSObjectRef getConstructor(NX::Context * context) {
          return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::ReadableStream::Constructor);
        }

        JSObjectRef sourceDevice() { return myDevice; }

      public:

        virtual JSValueRef read(JSContextRef ctx, JSObjectRef thisObject, std::size_t length, JSValueRef * exception);
        virtual JSValueRef readSync(JSContextRef ctx, JSObjectRef thisObject, std::size_t length, JSValueRef * exception);

        virtual JSValueRef pushReadFilter(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef readFilter,
                                          JSValueRef * exception)
        {
          try {
            NX::Object filter(NX::Context::FromJsContext(ctx)->toJSContext(), readFilter);
            myFilters.push_back(filter);
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
          return thisObject;
        }

        virtual JSValueRef popReadFilter(JSContextRef ctx, JSObjectRef thisObject, JSValueRef * exception) {
          try {
            NX::Object filter(myFilters.back());
            myFilters.pop_back();
            return filter;
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
        }

        virtual JSValueRef pipe(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef target, JSValueRef * exception) {
          std::string deviceType(myDevice["type"]->toString());
          if (deviceType == "pull") {
            NX::Object promise(ctx, this->read(ctx, thisObject, 0, exception));
            if (exception && *exception)
              return JSValueMakeUndefined(ctx);
            return promise["then"]->toObject()->call(promise, {
              NX::Object(ctx, target)["write"]->toObject()->bind(target, 0, nullptr, exception)
            }, exception);
          } else if (deviceType == "push") {
            NX::Object processFilters(ctx, JSObjectMakeFunctionWithCallback(
              ctx, nullptr, [](JSContextRef ctx, JSObjectRef function,
              JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
              JSValueRef* exception) -> JSValueRef
              {
                ReadableStream * stream = ReadableStream::FromObject(thisObject);
                JSValueRef promise = NX::Globals::Promise::resolve(ctx, arguments[1]);
                for(auto & i : stream->myFilters) {
                  NX::Object p(ctx, promise);
                  promise = p.then([=](JSContextRef ctx, JSValueRef value, JSValueRef * exception){
                    return i["process"]->toObject()->call(i, std::vector<JSValueRef> { value }, exception);
                  });
                  if (exception && *exception) break;
                }
                if (exception && *exception) return JSValueMakeUndefined(ctx);
                NX::Context * context = NX::Context::FromJsContext(ctx);
                NX::Object target(context->toJSContext(), arguments[0]);
                return NX::Object(context->toJSContext(), promise).then([=](auto ctx, auto value, auto exception) {
                  return target["write"]->toObject()->call(target, std::vector<JSValueRef> { value }, exception);
                });
              }
            ));
            JSValueRef argsForBind[] { target };
            JSValueRef boundProcessor = processFilters.bind(thisObject, 1, argsForBind, exception);
            if (exception && *exception)
              return JSValueMakeUndefined(ctx);
            myDevice["on"]->toObject()->call(myDevice, std::vector<JSValueRef> {
              NX::Value(ctx, "data").value(),
              boundProcessor
            }, exception);
            if (exception && *exception)
              return JSValueMakeUndefined(ctx);
            return NX::Object(ctx, myDevice["reset"]->toObject()->call(myDevice, std::vector<JSValueRef>(), exception))
              .then([=](JSContextRef ctx, JSValueRef arg, auto exception) {
                return myDevice["resume"]->toObject()->call(myDevice, std::vector<JSValueRef>(), exception);
              });
          }
          else {
            if (exception)
              *exception = NX::Object(ctx, std::runtime_error("invalid device type"));
          }
          return JSValueMakeUndefined(ctx);
        }

      private:
        NX::Object myDevice;
        std::vector<NX::Object> myFilters;
      };

      class WritableStream: public virtual NX::Classes::IO::Stream {
      private:
        static const JSClassDefinition Class;
        static const JSStaticValue Properties[];
        static const JSStaticFunction Methods[];

        static void Finalize(JSObjectRef object) {
        }

        static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                       const JSValueRef arguments[], JSValueRef* exception)
        {
          NX::Context * context = NX::Context::FromJsContext(ctx);
          JSClassRef _class = context->nexus()->defineOrGetClass(NX::Classes::IO::WritableStream::Class);
          try {
            if (argumentCount < 1) {
              throw std::runtime_error("missing parameter device in call to WritableStream constructor");
            }
            return JSObjectMake(ctx, _class, static_cast<NX::Classes::Base*>(new WritableStream(NX::Object(context->toJSContext(), arguments[0]))));
          } catch(const std::exception & e) {
            JSWrapException(ctx, e, exception);
            return JSObjectMake(ctx, nullptr, nullptr);
          }
        }

      public:
        WritableStream(const NX::Object & device): myDevice(device) {}
        virtual ~WritableStream() {}

        static JSClassRef createClass(NX::Context * context);

        static NX::Classes::IO::WritableStream * FromObject(JSObjectRef object) {
          return dynamic_cast<NX::Classes::IO::WritableStream *>(NX::Classes::Base::FromObject(object));
        }

        JSObjectRef sinkDevice() { return myDevice; }

        static JSObjectRef getConstructor(NX::Context * context) {
          return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::WritableStream::Constructor);
        }


      public:
        virtual JSValueRef write(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef buffer);
        virtual JSValueRef writeSync(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef buffer, JSValueRef * exception);

        virtual JSValueRef pushWriteFilter(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef writeFilter,
                                          JSValueRef * exception)
        {
          try {
            NX::Object filter(NX::Context::FromJsContext(ctx)->toJSContext(), writeFilter);
            myFilters.push_back(filter);
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
          return thisObject;
        }

        virtual JSValueRef popWriteFilter(JSContextRef ctx, JSObjectRef thisObject, JSValueRef * exception) {
          try {
            NX::Object filter(myFilters.back());
            myFilters.pop_back();
            return filter;
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
        }

      private:
        NX::Object myDevice;
        std::vector<NX::Object> myFilters;
      };

      class BidirectionalStream: public virtual NX::Classes::IO::ReadableStream, public virtual NX::Classes::IO::WritableStream
      {
      private:

      public:
        BidirectionalStream (const NX::Object & device): ReadableStream(device), WritableStream(device) {}
        virtual ~BidirectionalStream() {}

        static NX::Classes::IO::BidirectionalStream * FromObject(JSObjectRef object) {
          return reinterpret_cast<NX::Classes::IO::BidirectionalStream *>(JSObjectGetPrivate(object));
        }

      };
    }
  }
}

#endif // CLASSES_FILE_H
