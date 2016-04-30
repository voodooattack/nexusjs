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
#include "task.h"

namespace NX
{
  class Nexus;
  class Context;
  namespace Classes
  {
    namespace IO
    {
      namespace Devices
      {
        class FilePullDevice: public virtual SeekableSourceDevice {
        public:
          FilePullDevice(const std::string & path);
          virtual ~FilePullDevice() { myStream.close(); }

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

          static NX::Classes::IO::Devices::FilePullDevice * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::FilePullDevice*>(Base::FromObject(obj));
          }

          virtual std::size_t devicePosition() { return myStream.tellg(); }
          virtual std::size_t deviceRead ( char * dest, std::size_t length ) {
            return myStream.readsome(dest, length);
          }

          virtual bool deviceReady() const { return myStream.good(); }
          virtual std::size_t deviceSeek ( std::size_t pos, Position from ) {
            myStream.seekg(pos, (std::ios::seekdir)from);
            return myStream.tellg();
          }
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

        class FilePushDevice: public virtual PushSourceDevice {
        public:
          FilePushDevice(NX::Scheduler * scheduler, const std::string & path);
          virtual ~FilePushDevice() { myStream.close(); }

        private:
          static const JSClassDefinition Class;
          static const JSStaticValue Properties[];
          static const JSStaticFunction Methods[];

          static void Finalize(JSObjectRef object) { }

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef* exception) {
            NX::Context * context = NX::Context::FromJsContext(ctx);
            JSClassRef fileSourceClass = createClass(context);
            try {
              if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
                throw std::runtime_error("argument must be a string path");
              NX::Value path(ctx, arguments[0]);
              return JSObjectMake(ctx, fileSourceClass,
                                  dynamic_cast<NX::Classes::Base*>(
                                    new NX::Classes::IO::Devices::FilePushDevice(context->nexus()->scheduler(), path.toString())));
            } catch (const std::exception & e) {
              JSWrapException(ctx, e, exception);
              return JSObjectMake(ctx, nullptr, nullptr);
            }
          }

        public:
          static JSClassRef createClass(NX::Context * context) {
            JSClassDefinition def = NX::Classes::IO::Devices::FilePushDevice::Class;
            def.parentClass = NX::Classes::IO::PushSourceDevice::createClass (context);
            return context->nexus()->defineOrGetClass (def);
          }
          static JSObjectRef getConstructor(NX::Context * context) {
            return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::IO::Devices::FilePushDevice::Constructor);
          }

          static NX::Classes::IO::Devices::FilePushDevice * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::FilePushDevice*>(Base::FromObject(obj));
          }

          virtual bool deviceReady() const { return myStatus == State::Paused; }
          virtual bool eof() const { myStream.eof(); }
          virtual void pause(JSContextRef ctx, JSObjectRef thisObject) {
            if (NX::AbstractTask * task = myTask) task->abort();
          }
          virtual void reset(JSContextRef ctx, JSObjectRef thisObject) { pause(ctx, thisObject); myStream.seekg(0, std::ios::beg); }
          virtual JSObjectRef resume(JSContextRef ctx, JSObjectRef thisObject);
          virtual State state() const { return myStatus; }

        private:
          NX::Scheduler * myScheduler;
          boost::atomic<State> myStatus;
          boost::atomic<NX::AbstractTask*> myTask;
          std::ifstream myStream;
          NX::Object myPromise;
        };


        class FileSinkDevice: public virtual SeekableSinkDevice {

          FileSinkDevice(const std::string & path);
          virtual ~FileSinkDevice() { myStream.flush(); myStream.close(); }

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

          static NX::Classes::IO::Devices::FileSinkDevice * FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::FileSinkDevice*>(Base::FromObject(obj));
          }

          virtual std::size_t devicePosition() {
            return myStream.tellp();
          }
          virtual bool deviceReady() const {
            return myStream.good();
          }
          virtual std::size_t deviceSeek ( std::size_t pos, Position from ) {
            myStream.seekp(pos, (std::ios::seekdir)from);
            return myStream.tellp();
          }
          virtual void deviceWrite ( const char * buffer, std::size_t length ) {
            myStream.write(buffer, length);
            myStream.flush();
          }

        private:
          std::ofstream myStream;
        };

//         class BidirectionalFileDevice: public BidirectionalDualSeekableDevice {
//
//         };
      }
    }
  }
}

#endif // CLASSES_IO_DEVICES_FILE_H
