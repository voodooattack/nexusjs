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

#ifndef CLASSES_IO_DEVICES_FILE_H
#define CLASSES_IO_DEVICES_FILE_H

#include <JavaScript.h>
#include <fstream>

#include "classes/io/device.h"

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
  {
    namespace IO
    {
      class FileSourceDevice: public virtual SeekableSourceDevice {
      public:
        FileSourceDevice(const std::string & path);
        virtual ~FileSourceDevice() { myStream.close(); }

      private:
        static const JSClassDefinition Class;
        static const JSStaticValue Properties[];
        static const JSStaticFunction Methods[];

        static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                       const JSValueRef arguments[], JSValueRef* exception);

        static void Finalize(JSObjectRef object) { }

      public:
        static JSClassRef createClass(NX::Context * context);
        static JSObjectRef getConstructor(NX::Context * context);

        static NX::Classes::IO::FileSourceDevice * FromObject(JSObjectRef object) {
          NX::Classes::IO::Device * context = reinterpret_cast<NX::Classes::IO::Device *>(JSObjectGetPrivate(object));
          return dynamic_cast<NX::Classes::IO::FileSourceDevice*>(context);
        }

        virtual std::size_t devicePosition() { return myStream.tellg(); }
        virtual std::size_t deviceRead ( char * dest, std::size_t length ) {
          myStream.read(dest, length);
          return myStream.gcount();
        }

        virtual bool deviceReady() const { return !myStream.bad(); }
        virtual std::size_t deviceSeek ( std::size_t pos, Position from ) { myStream.seekg(pos, (std::ios::seekdir)from); }
        virtual bool eof() const { return myStream.eof(); }
        virtual std::size_t sourceSize() {
          std::size_t size = 0;
          std::streampos current = myStream.tellg();
          myStream.seekg(0, std::ios::beg);
          std::streampos beg = myStream.tellg();
          myStream.seekg(0, std::ios::end);
          std::streampos end = myStream.tellg();
          myStream.seekg(current, std::ios::beg);
          return end - beg;
        }
        virtual std::size_t deviceBytesAvailable() {
          return sourceSize() - myStream.tellg();
        }
      private:
        std::ifstream myStream;
      };

      class FileSinkDevice: public virtual SeekableSinkDevice {

      };

      class BidirectionalFileDevice: public BidirectionalDualSeekableDevice {

      };
    }
  }
}

#endif // CLASSES_IO_DEVICES_FILE_H
