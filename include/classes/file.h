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

#ifndef CLASSES_FILE_H
#define CLASSES_FILE_H

#include <JavaScript.h>
#include <fstream>

#include "classes/stream.h"

namespace NX
{
  class Nexus;
  class Module;
  namespace Classes
  {
    class File: public NX::Classes::Stream
    {
    private:
      static const JSClassDefinition Class;
      static const JSStaticValue Properties[];
      static const JSStaticFunction Methods[];

      static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                     const JSValueRef arguments[], JSValueRef* exception);

      static void Finalize(JSObjectRef object) {
        FromObject(object)->myStream.close();
      }

    public:
      static JSClassRef createClass(NX::Module * module);
      static JSObjectRef getConstructor(NX::Module * module);
      static JSValueRef create(NX::Module * module, JSContextRef ctx,
                               unsigned int argumentsCount, JSValueRef arguments[],
                               JSValueRef * exception);

      static NX::Classes::File * FromObject(JSObjectRef object) {
        /* Two-step cast is important. */
        NX::Classes::Stream * stream = reinterpret_cast<NX::Classes::Stream *>(JSObjectGetPrivate(object));
        return dynamic_cast<NX::Classes::File *>(stream);
      }

    public:
      File(NX::Module * owner, const std::string & fileName, std::fstream::openmode mode);
      virtual ~File() { }

      JSValueRef readAsBuffer(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                      size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception);
      JSValueRef readAsString(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                              size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception);
    private:
      NX::Module * myOwner;
      std::fstream myStream;
    };
  }
}

#endif // CLASSES_FILE_H
