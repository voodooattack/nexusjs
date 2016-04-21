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

#include "classes/io/device.h"
#include "classes/io/filter.h"
#include "util.h"

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
  {
    namespace IO
    {
      class Stream
      {
      private:
        static const JSClassDefinition Class;
        static const JSStaticValue Properties[];
        static const JSStaticFunction Methods[];

        static void Finalize(JSObjectRef object) {
          delete FromObject(object);
        }

      public:

        static NX::Classes::IO::Stream * FromObject(JSObjectRef object) {
          return reinterpret_cast<NX::Classes::IO::Stream *>(JSObjectGetPrivate(object));
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

      public:
        ReadableStream(const NX::Object & device): myDevice(device) { }
        virtual ~ReadableStream() {}

        static JSClassRef createClass(NX::Context * context);

        static NX::Classes::IO::ReadableStream * FromObject(JSObjectRef object) {
          return reinterpret_cast<NX::Classes::IO::ReadableStream *>(JSObjectGetPrivate(object));
        }

        JSObjectRef sourceDevice() { return myDevice; }

      public:

        virtual JSValueRef readAsBuffer(JSContextRef ctx, JSObjectRef thisObject, std::size_t length);
        virtual JSValueRef readAsString(JSContextRef ctx, JSObjectRef thisObject, const std::string & encoding,
                                        std::size_t length);

        virtual JSValueRef readAsBufferSync(JSContextRef ctx, JSObjectRef thisObject, std::size_t length,
                                            JSValueRef * exception);
        virtual JSValueRef readAsStringSync(JSContextRef ctx, JSObjectRef thisObject, const std::string & encoding,
                                        std::size_t length, JSValueRef * exception);

        virtual JSValueRef pushReadFilter(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef readFilter,
                                          JSValueRef * exception)
        {
          try {
            NX::Object filter(ctx, readFilter);
            myFilters.push_back(filter);
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
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

      public:
        WritableStream(const NX::Object & device): myDevice(device) {}
        virtual ~WritableStream() {}

        static JSClassRef createClass(NX::Context * context);

        static NX::Classes::IO::WritableStream * FromObject(JSObjectRef object) {
          return reinterpret_cast<NX::Classes::IO::WritableStream *>(JSObjectGetPrivate(object));
        }

        JSObjectRef sinkDevice() { return myDevice; }

      public:
        virtual JSValueRef writeString(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef writable, const std::string & encoding);
        virtual JSValueRef writeBuffer(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef buffer);

        virtual JSValueRef writeStringSync(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef writable, const std::string & encoding,
                                          JSValueRef * exception);
        virtual JSValueRef writeBufferSync(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef buffer, JSValueRef * exception);

        virtual JSValueRef pushWriteFilter(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef writeFilter,
                                          JSValueRef * exception)
        {
          try {
            NX::Object filter(ctx, writeFilter);
            myFilters.push_back(filter);
          } catch(const std::exception & e) {
            return JSWrapException(ctx, e, exception);
          }
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
