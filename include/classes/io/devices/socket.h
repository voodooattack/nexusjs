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

#ifndef CLASSES_IO_DEVICES_SOCKET_H
#define CLASSES_IO_DEVICES_SOCKET_H

#include "classes/io/device.h"
#include <JavaScript.h>

namespace NX {
  namespace Classes {
    namespace IO {
      namespace Devices {
        class Socket: public virtual BidirectionalDevice {
          public:
            Socket();
            virtual ~Socket() { }

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

            static NX::Classes::IO::Devices::Socket * FromObject(JSObjectRef object) {
              NX::Classes::IO::Device * context = reinterpret_cast<NX::Classes::IO::Device *>(JSObjectGetPrivate(object));
              return dynamic_cast<NX::Classes::IO::Devices::Socket*>(context);
            }

            virtual bool eof() const;
            virtual bool deviceReady() const;
            virtual std::size_t deviceRead ( char * dest, std::size_t length );
            virtual void deviceWrite ( const char * buffer, std::size_t length );

        };
      }
    }
  }
}

#endif // CLASSES_IO_DEVICES_SOCKET_H
