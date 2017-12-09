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


#ifndef CLASSES_NET_HTCOMMON_REQUEST_H
#define CLASSES_NET_HTCOMMON_REQUEST_H

#include <JavaScript.h>

#include "classes/net/htcommon/connection.h"
#include "classes/io/device.h"

namespace NX {
  namespace Classes {
    namespace Net {
      namespace HTCommon {
        class Request: public NX::Classes::IO::PushSourceDevice {
        protected:
          Request (NX::Classes::Net::HTCommon::Connection * connection):
              NX::Classes::IO::PushSourceDevice(), myConnection(connection)
          {
          }

        public:
          virtual ~Request() {}

          static NX::Classes::Net::HTCommon::Request * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::Net::HTCommon::Request*>(NX::Classes::Base::FromObject(obj));
          }

          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::Net::HTCommon::Request::Class;
            def.parentClass = NX::Classes::IO::PushSourceDevice::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }

          const boost::system::error_code & deviceError() const override { return myError; }

          void deviceClose() override { }

          virtual JSValueRef attach(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef connection) = 0;

          static const JSClassDefinition Class;
          static const JSStaticFunction Methods[];
          static const JSStaticValue Properties[];

          virtual boost::system::error_code & error() { return myError; }

          NX::Classes::Net::HTCommon::Connection * connection() { return myConnection; }
        private:
          NX::Classes::Net::HTCommon::Connection * myConnection;
          boost::system::error_code myError;
        };
      }
    }
  }
}

#endif
