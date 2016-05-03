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
#include "globals/promise.h"

#include <boost/filesystem.hpp>

NX::Classes::IO::Devices::FilePullDevice::FilePullDevice (const std::string & path): myStream(path, std::ifstream::binary) {
  if (!boost::filesystem::exists(path))
    throw std::runtime_error("file '" + path + "' not found");
}

JSObjectRef NX::Classes::IO::Devices::FilePullDevice::Constructor (JSContextRef ctx, JSObjectRef constructor,
                                                            size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef fileSourceClass = createClass(context);
  try {
    if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
      throw std::runtime_error("argument must be a string path");
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
    if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
      throw std::runtime_error("argument must be a string path");
    NX::Value path(ctx, arguments[0]);
    return JSObjectMake(ctx, fileSourceClass, dynamic_cast<NX::Classes::Base*>(new NX::Classes::IO::Devices::FileSinkDevice(path.toString())));
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

NX::Classes::IO::Devices::FilePushDevice::FilePushDevice (NX::Scheduler * scheduler, const std::string & path) :
  myScheduler(scheduler), myState(Paused), myTask(nullptr), myStream(path, std::ifstream::binary), myPromise(), myMutex()
{
  if (!boost::filesystem::exists (path))
  {
    throw std::runtime_error ("file '" + path + "' not found");
  }
}

JSObjectRef NX::Classes::IO::Devices::FilePushDevice::resume (JSContextRef ctx, JSObjectRef thisObject)
{
  if (myState == Paused)
  {
    myState = Resumed;
    NX::Context * context = NX::Context::FromJsContext (ctx);
    JSValueProtect(context->toJSContext(), thisObject);
    JSObjectRef promise = NX::Globals::Promise::createPromise (context->toJSContext(),
      [=](NX::Context *, ResolveRejectHandler resolve, ResolveRejectHandler reject)
    {
      auto readHandler = [=](auto readHandler) {
        boost::recursive_mutex::scoped_lock lock(myMutex);
        if(myState == Resumed) {
          const std::size_t maxSize = std::size_t(1024 * 1024);
          char * buffer = (char*)std::malloc(maxSize);
          myStream.read(buffer, maxSize);
          std::size_t sizeOut = myStream.gcount();
          if (sizeOut) {
            JSValueRef exp = nullptr;
            lock.unlock();
            JSObjectRef arrayBuffer = JSObjectMakeArrayBufferWithBytesNoCopy(context->toJSContext(), buffer, sizeOut,
                                                                              [](void * ptr, void * ctx) {
                                                                                std::free(ptr);
                                                                              }, nullptr, &exp);
            JSValueRef args[] { arrayBuffer };
            this->emitFast(context->toJSContext(), thisObject, "data", 1, args, &exp);
            if (exp) {
              std::free(buffer);
              reject(exp);
              myState = Paused;
              myPromise = NX::Object();
              JSValueUnprotect(context->toJSContext(), thisObject);
              return;
            }
            lock.lock();
          } else {
            std::free(buffer);
          }
        }
        if (myStream.eof()) {
          lock.unlock();
          resolve(thisObject);
          JSValueUnprotect(context->toJSContext(), thisObject);
          myState = Paused;
          myPromise = NX::Object();
        } else {
          lock.unlock();
          myScheduler->scheduleTask(std::bind(readHandler, std::move(readHandler)));
        }
      };
      myScheduler->scheduleTask(std::bind(readHandler, std::move(readHandler)));
    });
    myPromise = NX::Object (context->toJSContext(), promise);
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
