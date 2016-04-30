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

NX::Classes::IO::Devices::FilePushDevice::FilePushDevice (NX::Scheduler * scheduler, const std::string & path) :
  myScheduler (scheduler), myStream (path, std::ifstream::binary), myStatus (Paused), myTask (nullptr), myPromise()
{
  if (!boost::filesystem::exists (path))
  {
    throw std::runtime_error ("file '" + path + "' not found");
  }
}

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

JSObjectRef NX::Classes::IO::Devices::FilePushDevice::resume (JSContextRef ctx, JSObjectRef thisObject)
{
  if (myStatus == Paused)
  {
    myStatus = Resumed;
    NX::Context * context = NX::Context::FromJsContext (ctx);
    JSValueProtect (context->toJSContext(), thisObject);
    JSObjectRef promise = NX::Globals::Promise::createPromise (ctx, [ = ] (NX::Context *, ResolveRejectHandler resolve, ResolveRejectHandler reject)
    {
      const std::size_t maxBufferSize = 1024 * 1024;
      NX::AbstractTask * task = myScheduler->scheduleCoroutine ([ = ]()
      {
        char * buffer = (char *) std::malloc (maxBufferSize);

        while (myStream.good())
        {
          std::size_t pos = myStream.tellg();
          std::size_t toRead = std::min ( (std::size_t) myStream.rdbuf()->in_avail(), maxBufferSize);

          if (!toRead)
          {
            toRead = maxBufferSize;
          }

          if (!buffer)
          {
            buffer = (char *) std::malloc (toRead);
          }

          std::size_t sizeOut = 0;
          sizeOut = myStream.readsome (buffer, toRead);

          if (!sizeOut)
          {
            myStream.read (buffer, toRead);
            sizeOut = myStream.gcount();
          }

          if (sizeOut < toRead && sizeOut)
          {
            buffer = (char *) std::realloc (buffer, sizeOut);
          }

          bool eof = myStream.eof();

          if (sizeOut)
          {
            myScheduler->scheduleTask ([ = ]()
            {
              JSValueRef args[]
              {
                JSObjectMakeArrayBufferWithBytesNoCopy (context->toJSContext(), buffer, sizeOut,
                [] (void * bytes, void * deallocatorContext)
                {
                  std::free (bytes);
                }, nullptr, nullptr),
                JSValueMakeNumber (context->toJSContext(), pos)
              };
              this->emit (context->toJSContext(), thisObject, "data", 2, args, nullptr);
            });
            buffer = nullptr;
          }

          myScheduler->yield();
        }

        myScheduler->scheduleTask ([ = ]()
        {
          resolve (thisObject);
          JSValueUnprotect (context->toJSContext(), thisObject);
        });
      });
      task->addCancellationHandler ([ = ]()
      {
        myTask.store (nullptr);
        JSValueUnprotect (context->toJSContext(), thisObject);
      });
      task->addCompletionHandler ([this]()
      {
        myTask.store (nullptr);
      });
      myTask.store (task);
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
