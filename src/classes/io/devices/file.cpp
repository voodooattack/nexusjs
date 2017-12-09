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

NX::Classes::IO::Devices::FilePullDevice::FilePullDevice (const std::string & path): myStream(path, std::ifstream::in | std::ifstream::binary) {
  if (!boost::filesystem::exists(path))
    throw NX::Exception("file '" + path + "' not found");
  myStream.unsetf(std::ios_base::skipws);
}

JSObjectRef NX::Classes::IO::Devices::FilePullDevice::Constructor (JSContextRef ctx, JSObjectRef constructor,
                                                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef fileSourceClass = createClass(context);
  try {
    if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
      throw NX::Exception("argument must be a string path");
    NX::Value path(ctx, arguments[0]);
    return JSObjectMake(ctx, fileSourceClass, dynamic_cast<NX::Classes::Base*>(new NX::Classes::IO::Devices::FilePullDevice(path.toString())));
  } catch (const std::exception & e) {
    JSWrapException(ctx, e, exception);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
}

JSClassRef NX::Classes::IO::Devices::FilePullDevice::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::IO::Devices::FilePullDevice::Class;
  def.parentClass = NX::Classes::IO::SeekableSourceDevice::createClass (context);
  return context->nexus()->defineOrGetClass (def);
}

JSObjectRef NX::Classes::IO::Devices::FilePullDevice::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Devices::FilePullDevice::Constructor);
}

const JSClassDefinition NX::Classes::IO::Devices::FilePullDevice::Class {
  0, kJSClassAttributeNone, "FileSourceDevice", nullptr, NX::Classes::IO::Devices::FilePullDevice::Properties,
  NX::Classes::IO::Devices::FilePullDevice::Methods, nullptr, NX::Classes::IO::Devices::FilePullDevice::Finalize
};

const JSStaticValue NX::Classes::IO::Devices::FilePullDevice::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::Devices::FilePullDevice::Methods[] {
  { nullptr, nullptr, 0 }
};


const JSStaticValue NX::Classes::IO::Devices::FilePushDevice::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::IO::Devices::FilePushDevice::Methods[] {
  { nullptr, nullptr, 0 }
};

const JSClassDefinition NX::Classes::IO::Devices::FilePushDevice::Class {
  0, kJSClassAttributeNone, "FilePushDevice", nullptr, NX::Classes::IO::Devices::FilePushDevice::Properties,
  NX::Classes::IO::Devices::FilePushDevice::Methods, nullptr, NX::Classes::IO::Devices::FilePushDevice::Finalize
};


JSObjectRef NX::Classes::IO::Devices::FileSinkDevice::Constructor (JSContextRef ctx, JSObjectRef constructor,
                                                          size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef fileSourceClass = createClass(context);
  try {
    auto type = JSValueGetType(ctx, arguments[0]);
    if (argumentCount < 1 || (type != kJSTypeString && type != kJSTypeNumber))
      *exception = NX::Exception("argument must be a string path or file descriptor").toError(ctx);
    NX::Value pathOrFD(ctx, arguments[0]);
    if (type == kJSTypeString)
      return JSObjectMake(ctx, fileSourceClass, dynamic_cast<NX::Classes::Base*>(new NX::Classes::IO::Devices::FileSinkDevice(
        pathOrFD.toString()
      )));
    else {
      auto fd = static_cast<int>(pathOrFD.toNumber());
      return JSObjectMake(ctx, fileSourceClass,
                          dynamic_cast<NX::Classes::Base *>(new NX::Classes::IO::Devices::FileSinkDevice(
                            fd, false
                          )));
    }
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

NX::Classes::IO::Devices::FileSinkDevice::FileSinkDevice (const std::string & path): myStream()
{
  std::ios_base::iostate exceptionMask = myStream.exceptions() | std::ios::failbit | std::ios::badbit;
  myStream.exceptions(exceptionMask);
  myStream.open(boost::iostreams::file_descriptor(path, std::ios::out | std::ios::binary | std::ios::trunc));
}

NX::Classes::IO::Devices::FileSinkDevice::FileSinkDevice(int fd, bool close): myStream() {
  std::ios_base::iostate exceptionMask = myStream.exceptions() | std::ios::failbit | std::ios::badbit;
  myStream.exceptions(exceptionMask);
  myStream.open(boost::iostreams::file_descriptor(fd, close ? boost::iostreams::close_handle : boost::iostreams::never_close_handle));
}


JSObjectRef NX::Classes::IO::Devices::FileSinkDevice::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Devices::FileSinkDevice::Constructor);
}

NX::Classes::IO::Devices::FilePushDevice::FilePushDevice (NX::Scheduler * scheduler, const std::string & path) :
  myScheduler(scheduler), myPath(path), myState(Paused), myTask(nullptr), myStream(path, std::ios_base::in | std::ios_base::binary), myPromise()
{
}

JSObjectRef NX::Classes::IO::Devices::FilePushDevice::resume (JSContextRef ctx, JSObjectRef thisObject)
{
  if (myState == Paused)
  {
    NX::Context * context = NX::Context::FromJsContext (ctx);
    if (!myStream.is_open()) {
      myStream.open(myPath, std::ios_base::in | std::ios_base::binary);
    }
    myState = Resumed;
    NX::Object thisObj(context->toJSContext(), thisObject);
    myPromise = NX::Object(context->toJSContext(), NX::Globals::Promise::createPromise(context->toJSContext(),
      [=](JSContextRef ctx, NX::ResolveRejectHandler resolve, NX::ResolveRejectHandler reject) -> JSValueRef
      {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        auto readHandler = [=](auto readHandler) {
          if (myState == Resumed) {
            try {
              auto buffer = (char *) WTF::fastMalloc(FILE_PUSH_DEVICE_BUFFER_SIZE);
              auto sizeOut = static_cast<size_t>(myStream.readsome(buffer, FILE_PUSH_DEVICE_BUFFER_SIZE));
              if (!sizeOut) {
                myStream.read(buffer, FILE_PUSH_DEVICE_BUFFER_SIZE);
                sizeOut = static_cast<size_t>(myStream.gcount());
              }
              if (sizeOut) {
                if (sizeOut < FILE_PUSH_DEVICE_BUFFER_SIZE)
                  buffer = static_cast<char *>(WTF::fastRealloc(buffer, sizeOut));
                JSValueRef exp = nullptr;
                JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(
                  context->toJSContext(), buffer, sizeOut,
                  [](void *ptr, void *) {
                    WTF::fastFree(ptr);
                  }, this, &exp);
                if (exp) {
                  myState = Paused;
                  WTF::fastFree(buffer);
                  reject(context->toJSContext(), exp);
                  return;
                }
                JSValueRef args[]{arrayBuffer};
                NX::Object(context->toJSContext(), this->emit(context->toJSContext(), thisObj, "data", 1, args, &exp))
                  .then([=](JSContextRef ctx, JSValueRef arg, JSValueRef *exception) {
                    if (!myStream.eof()) {
                      myScheduler->scheduleTask(std::move(std::bind<void>(readHandler, readHandler)));
                    } else {
                      emitFast(context->toJSContext(), thisObj, "end", 0, nullptr, nullptr);
                      resolve(ctx, thisObj);
                    }
                    return arg;
                  }, [=](JSContextRef ctx, JSValueRef arg, JSValueRef *exception) {
                    myState = Paused;
                    JSValueRef args[] { arg };
                    emitFast(context->toJSContext(), thisObj, "error", 1, args, nullptr);
                    reject(ctx, arg);
                    return arg;
                  });
                if (exp) {
                  myState = Paused;
                  JSValueRef args[] { exp };
                  emitFast(context->toJSContext(), thisObj, "error", 1, args, nullptr);
                  reject(context->toJSContext(), exp);
                  return;
                }
              } else {
                WTF::fastFree(buffer);
                if (myStream.eof()) {
                  myState = Paused;
                  this->emitFast(context->toJSContext(), thisObj, "end", 0, nullptr, nullptr);
                  resolve(context->toJSContext(), thisObj);
                  return;
                } else {
                  myScheduler->scheduleTask(std::move(std::bind<void>(readHandler, readHandler)));
                }
              }
            } catch(const std::exception &e) {
              reject(context->toJSContext(), NX::Object(context->toJSContext(), e));
            }
          } else
            myScheduler->scheduleTask(std::bind<void>(readHandler, readHandler));
        };
        myScheduler->scheduleTask(std::bind(readHandler, readHandler));
        return JSValueMakeUndefined(ctx);
    }));
  }
  return myPromise;
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
