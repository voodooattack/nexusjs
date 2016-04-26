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

#ifndef CLASSES_CONTEXT_H
#define CLASSES_CONTEXT_H

#include <JavaScript.h>
#include <boost/shared_ptr.hpp>

namespace NX {
  class Context;
  namespace Classes {
    class Context
    {
    private:
      static const JSClassDefinition Class;
      static const JSStaticValue Properties[];
      static const JSStaticFunction Methods[];

      static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                     const JSValueRef arguments[], JSValueRef* exception);

      static void Finalize(JSObjectRef object) {
        delete FromObject(object);
      }

    public:
      static JSClassRef createClass(NX::Context * context);
      static JSObjectRef getConstructor(NX::Context * context);

      static NX::Classes::Context * FromObject(JSObjectRef object) {
        /* Two-step cast is important. */
        NX::Classes::Context * context = reinterpret_cast<NX::Classes::Context *>(JSObjectGetPrivate(object));
        return context;
      }

    public:
      Context(NX::Context * parent, JSObjectRef globalOverrides = nullptr);
      virtual ~Context();

      JSValueRef eval(JSContextRef ctx, JSObjectRef thisObject, const std::string & source,
                      const std::string & fileName, unsigned int lineNo, JSValueRef * exception);
      JSValueRef exports(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception);

      std::shared_ptr<NX::Context> context() { return myContext; }

    private:
      std::shared_ptr<NX::Context> myContext;
    };
  }
}

#endif // CLASSES_CONTEXT_H
