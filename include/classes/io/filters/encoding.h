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

#ifndef CLASSES_IO_FILTERS_ENCODING_H
#define CLASSES_IO_FILTERS_ENCODING_H

#include "classes/io/filter.h"

namespace NX {
  namespace Classes {
    namespace IO {
      class EncodingConversionFilter: public NX::Classes::IO::Filter
      {
        static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                       const JSValueRef arguments[], JSValueRef* exception)
        {
          NX::Context * context = NX::Context::FromJsContext(ctx);
          JSClassRef filterClass = createClass(context);
          try {
            if (argumentCount != 2)
              throw std::runtime_error("invalid arguments");
            std::string from(NX::Value(ctx, arguments[0]).toString());
            std::string to(NX::Value(ctx, arguments[1]).toString());
            return JSObjectMake(ctx, filterClass, new EncodingConversionFilter(from, to));
          } catch(const std::exception & e) {
            JSWrapException(ctx, e, exception);
            return JSObjectMake(ctx, nullptr, nullptr);
          }
        }

      public:
        EncodingConversionFilter (const std::string & fromEncoding,
                                  const std::string & toEncoding);
        virtual ~EncodingConversionFilter() {}

        virtual std::size_t processBuffer(char * buffer, std::size_t length, char * dest = nullptr, std::size_t outLength = 0 );

        static NX::Classes::IO::EncodingConversionFilter * FromObject(JSObjectRef obj) {
          auto filter = reinterpret_cast<NX::Classes::IO::Filter*>(JSObjectGetPrivate(obj));
          return dynamic_cast<EncodingConversionFilter*>(filter);
        }

        static JSObjectRef getConstructor(NX::Context * context) {
          return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::EncodingConversionFilter::Constructor);
        }

      protected:
        std::string myEncodingFrom, myEncodingTo;
      };
    }
  }
}

#endif // CLASSES_IO_FILTERS_ENCODING_H
