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

#ifndef CLASSES_IO_UTF8STRINGFILTER_H
#define CLASSES_IO_UTF8STRINGFILTER_H

#include "classes/io/filter.h"

namespace NX {
  namespace Classes {
    namespace IO {
      namespace Filters {
        class UTF8StringFilter: public NX::Classes::IO::Filter
        {
          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                        const JSValueRef arguments[], JSValueRef* exception)
          {
            NX::Context * context = NX::Context::FromJsContext(ctx);
            JSClassRef filterClass = createClass(context);
            try {
              return JSObjectMake(ctx, filterClass, dynamic_cast<NX::Classes::Base*>(new UTF8StringFilter()));
            } catch(const std::exception & e) {
              JSWrapException(ctx, e, exception);
              return JSObjectMake(ctx, nullptr, nullptr);
            }
          }

          static JSClassRef createClass(NX::Context * context);

          static JSStaticFunction Methods[];

        public:
          UTF8StringFilter () {}
          virtual ~UTF8StringFilter() {}

          virtual std::size_t estimateOutputLength(const char * buffer, std::size_t length) { return length; }
          virtual std::size_t processBuffer(const char * buffer, std::size_t length, char * dest, std::size_t outLength) { return 0; }

          static NX::Classes::IO::Filters::UTF8StringFilter * FromObject(JSObjectRef obj) {
            auto filter = reinterpret_cast<NX::Classes::IO::Filter*>(JSObjectGetPrivate(obj));
            return dynamic_cast<NX::Classes::IO::Filters::UTF8StringFilter*>(filter);
          }

          static JSObjectRef getConstructor(NX::Context * context) {
            return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Filters::UTF8StringFilter::Constructor);
          }
        };
      }
    }
  }
}

#endif // CLASSES_IO_UTF8STRINGFILTER_H
