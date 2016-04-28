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

#ifndef CLASSES_IO_FILTER_H
#define CLASSES_IO_FILTER_H

#include <JavaScript.h>
#include "nexus.h"

#include "classes/base.h"
#include "classes/io/device.h"

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
  {
    namespace IO
    {
      struct Filter: public virtual NX::Classes::Base
      {
      private:
        static void Finalize(JSObjectRef object) {
          delete FromObject(object);
        }
      protected:
        Filter() {}

        static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                       const JSValueRef arguments[], JSValueRef* exception)
        {
          NX::Context * context = NX::Context::FromJsContext(ctx);
          return JSObjectMake(ctx, createClass(context), nullptr);
        }

      public:
        virtual ~Filter() {}

        virtual std::size_t processBuffer(char * buffer, std::size_t length, char * dest = nullptr, std::size_t outLength = 0) = 0;

        static NX::Classes::IO::Filter * FromObject(JSObjectRef obj) {
          return dynamic_cast<NX::Classes::IO::Filter*>(NX::Classes::Base::FromObject(obj));
        }

        static JSObjectRef getConstructor(NX::Context * context) {
          return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Filter::Constructor);
        }

        static JSClassRef createClass(NX::Context * context);

        static JSClassDefinition Class;
        static JSStaticFunction Methods[];
        static JSStaticValue Properties[];
      };
    }
  }
}

#endif // CLASSES_IO_FILTER_H
