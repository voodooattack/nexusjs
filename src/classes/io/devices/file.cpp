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
#include "nexus.h"
#include "classes/io/device.h"
#include "classes/io/devices/file.h"
#include <boost/filesystem.hpp>

NX::Classes::IO::Devices::FileSourceDevice::FileSourceDevice (const std::string & path): myStream(path, std::ifstream::binary) {
  if (!boost::filesystem::exists(path))
    throw std::runtime_error("file '" + path + "' not found");
}

JSObjectRef NX::Classes::IO::Devices::FileSourceDevice::Constructor (JSContextRef ctx, JSObjectRef constructor,
                                                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef fileSourceClass = createClass(context);
  try {
    if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
      throw std::runtime_error("argument must be a string path");
    NX::Value path(ctx, arguments[0]);
    return JSObjectMake(ctx, fileSourceClass, dynamic_cast<NX::Classes::IO::Device*>(new NX::Classes::IO::Devices::FileSourceDevice(path.toString())));
  } catch (const std::exception & e) {
    JSWrapException(ctx, e, exception);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
}

JSClassRef NX::Classes::IO::Devices::FileSourceDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::Devices::FileSourceDevice::Class;
  def.parentClass = NX::Classes::IO::SeekableSourceDevice::createClass (context);
  return context->nexus()->defineOrGetClass (def);
}

JSObjectRef NX::Classes::IO::Devices::FileSourceDevice::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Devices::FileSourceDevice::Constructor);
}

const JSClassDefinition NX::Classes::IO::Devices::FileSourceDevice::Class {
  0, kJSClassAttributeNone, "FileSourceDevice", nullptr, NX::Classes::IO::Devices::FileSourceDevice::Properties,
  NX::Classes::IO::Devices::FileSourceDevice::Methods, nullptr, NX::Classes::IO::Devices::FileSourceDevice::Finalize
};

const JSStaticValue NX::Classes::IO::Devices::FileSourceDevice::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::Devices::FileSourceDevice::Methods[] {
  { nullptr, nullptr, 0 }
};

JSObjectRef NX::Classes::IO::Devices::FileSinkDevice::Constructor (JSContextRef ctx, JSObjectRef constructor,
                                                          size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef fileSourceClass = createClass(context);
  try {
    if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
      throw std::runtime_error("argument must be a string path");
    NX::Value path(ctx, arguments[0]);
    return JSObjectMake(ctx, fileSourceClass, dynamic_cast<NX::Classes::IO::Device*>(new NX::Classes::IO::Devices::FileSinkDevice(path.toString())));
  } catch (const std::exception & e) {
    JSWrapException(ctx, e, exception);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
}

JSClassRef NX::Classes::IO::Devices::FileSinkDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::Devices::FileSinkDevice::Class;
  def.parentClass = NX::Classes::IO::SeekableSinkDevice::createClass (context);
  return context->nexus()->defineOrGetClass (def);
}

NX::Classes::IO::Devices::FileSinkDevice::FileSinkDevice (const std::string & path): myStream(path, std::ofstream::binary)
{

}

JSObjectRef NX::Classes::IO::Devices::FileSinkDevice::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Devices::FileSinkDevice::Constructor);
}

const JSClassDefinition NX::Classes::IO::Devices::FileSinkDevice::Class {
  0, kJSClassAttributeNone, "FileSinkDevice", nullptr, NX::Classes::IO::Devices::FileSinkDevice::Properties,
  NX::Classes::IO::Devices::FileSinkDevice::Methods, nullptr, NX::Classes::IO::Devices::FileSinkDevice::Finalize
};

const JSStaticValue NX::Classes::IO::Devices::FileSinkDevice::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::Devices::FileSinkDevice::Methods[] {
  { nullptr, nullptr, 0 }
};
