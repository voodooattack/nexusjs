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

#ifndef CLASSES_IO_DEVICE_H
#define CLASSES_IO_DEVICE_H

#include <JavaScript.h>
#include <iosfwd>

#include "context.h"

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
  {
    namespace IO
    {
      struct Device {
      private:
        static void Finalize(JSObjectRef object) {
          delete FromObject(object);
        }
      protected:
        Device() {}
      public:
        enum Position { Beginning = std::ios::beg, Current = std::ios::cur, End = std::ios::end };

        virtual ~Device() {}

        virtual bool deviceReady() const = 0;

        static NX::Classes::IO::Device * FromObject(JSObjectRef obj) {
          return reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
        }

        static JSClassRef createClass(NX::Context * context);

        static JSStaticFunction Methods[];
        static JSStaticValue Properties[];
      };

      struct SourceDevice: public virtual Device {
        virtual std::size_t deviceRead(char * dest, std::size_t length) = 0;
        virtual bool eof() const = 0;

        static NX::Classes::IO::SourceDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::SourceDevice *>(dev);
        }

        static JSClassRef createClass(NX::Context * context);

        static JSStaticFunction Methods[];
      };

      struct SinkDevice: public virtual Device {
        virtual void deviceWrite(const char * buffer, std::size_t length) = 0;

        static NX::Classes::IO::SinkDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::SinkDevice *>(dev);
        }

        static JSClassRef createClass(NX::Context * context);

        static JSStaticFunction Methods[];
      };

      struct BidirectionalDevice: public virtual SourceDevice, public virtual SinkDevice {

        static NX::Classes::IO::BidirectionalDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::BidirectionalDevice *>(dev);
        }

        static JSClassRef createClass(NX::Context * context);

      };

      struct SeekableDevice: public virtual Device {
        virtual std::size_t devicePosition() = 0;
        virtual std::size_t deviceSeek(std::size_t pos, Position from) = 0;

        static NX::Classes::IO::SeekableDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::SeekableDevice *>(dev);
        }

        static JSClassRef createClass(NX::Context * context);

        static JSStaticFunction Methods[];
      };

      struct SeekableSourceDevice: public virtual SourceDevice, public virtual SeekableDevice {
        static NX::Classes::IO::SeekableSourceDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::SeekableSourceDevice *>(dev);
        }
        virtual std::size_t sourceSize() = 0;
        virtual std::size_t deviceBytesAvailable() = 0;
        static JSClassRef createClass(NX::Context * context);
      };

      struct SeekableSinkDevice: public virtual SinkDevice, public virtual SeekableDevice {
        static NX::Classes::IO::SeekableSinkDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::SeekableSinkDevice *>(dev);
        }
        virtual std::size_t sinkSize() = 0;
        static JSClassRef createClass(NX::Context * context);
      };

      struct BidirectionalSeekableDevice: public virtual SeekableSourceDevice, public virtual SeekableSinkDevice {
        static NX::Classes::IO::BidirectionalSeekableDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::BidirectionalSeekableDevice *>(dev);
        }

        static JSClassRef createClass(NX::Context * context);
      };

      struct DualSeekableDevice: public virtual Device {
        virtual std::size_t deviceReadPosition() = 0;
        virtual std::size_t deviceWritePosition() = 0;
        virtual std::size_t deviceReadSeek(std::size_t pos, Position from) = 0;
        virtual std::size_t deviceWriteSeek(std::size_t pos, Position from) = 0;

        static NX::Classes::IO::DualSeekableDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::DualSeekableDevice *>(dev);
        }

        static JSClassRef createClass(NX::Context * context);

        static JSStaticFunction Methods[];
      };

      struct BidirectionalDualSeekableDevice: public virtual DualSeekableDevice, public virtual BidirectionalDevice {
        static NX::Classes::IO::BidirectionalDualSeekableDevice * FromObject(JSObjectRef obj) {
          NX::Classes::IO::Device * dev = reinterpret_cast<NX::Classes::IO::Device*>(JSObjectGetPrivate(obj));
          return dynamic_cast<NX::Classes::IO::BidirectionalDualSeekableDevice *>(dev);
        }
        static JSClassRef createClass(NX::Context * context);
      };
    }
  }
}

#endif // CLASSES_IO_DEVICE_H
