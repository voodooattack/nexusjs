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
      class ReadableStream {
      public:
        static JSObjectRef getConstructor(NX::Context * context);
      };

      class WritableStream {
      public:
        static JSObjectRef getConstructor(NX::Context * context);
      };
    }
  }
}

#endif // CLASSES_FILE_H
