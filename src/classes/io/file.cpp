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

#include "util.h"
#include "classes/io/device.h"
#include "classes/io/file.h"


NX::Classes::IO::FileSourceDevice::FileSourceDevice (const std::string & path): myStream(path, std::ifstream::binary) { }

JSObjectRef NX::Classes::IO::FileSourceDevice::Constructor (JSContextRef ctx, JSObjectRef constructor,
                                                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef fileSourceClass = createClass(context);
  try {
    if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
      throw std::runtime_error("argument must be a string path");
    NX::Value path(ctx, arguments[0]);
    return JSObjectMake(ctx, fileSourceClass, new NX::Classes::IO::FileSourceDevice(path.toString()));
  } catch (const std::exception & e) {
    JSWrapException(ctx, e, exception);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
}

JSClassRef NX::Classes::IO::FileSourceDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::FileSourceDevice::Class;
  def.parentClass = NX::Classes::IO::SeekableSourceDevice::createClass (context);
  return context->defineOrGetClass (def);
}

JSObjectRef NX::Classes::IO::FileSourceDevice::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::FileSourceDevice::Constructor);
}

const JSClassDefinition NX::Classes::IO::FileSourceDevice::Class {
  0, kJSClassAttributeNone, "FileSourceDevice", nullptr, NX::Classes::IO::FileSourceDevice::Properties,
  NX::Classes::IO::FileSourceDevice::Methods, nullptr, NX::Classes::IO::FileSourceDevice::Finalize
};

const JSStaticValue NX::Classes::IO::FileSourceDevice::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::FileSourceDevice::Methods[] {
  { nullptr, nullptr, 0 }
};
