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

#ifndef CLASSES_STREAM_H
#define CLASSES_STREAM_H

#include <JavaScript.h>

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
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

      static NX::Classes::Stream * FromObject(JSObjectRef object) {
        return reinterpret_cast<NX::Classes::Stream *>(JSObjectGetPrivate(object));
      }

    public:
      static JSClassRef createClass(NX::Context * module);

      virtual JSValueRef readAsBuffer(JSContextRef ctx, JSObjectRef thisObject, std::size_t length) = 0;
      virtual JSValueRef readAsString(JSContextRef ctx, JSObjectRef thisObject, const std::string & encoding,
                                      std::size_t length) = 0;

      virtual JSValueRef readAsBufferSync(JSContextRef ctx, JSObjectRef thisObject, std::size_t length,
                                          JSValueRef * exception) = 0;
      virtual JSValueRef readAsStringSync(JSContextRef ctx, JSObjectRef thisObject, const std::string & encoding,
                                      std::size_t length, JSValueRef * exception) = 0;

    public:
      Stream() {}
      virtual ~Stream() {}
    };
  }
}

#endif // CLASSES_FILE_H
