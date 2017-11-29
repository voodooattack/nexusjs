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
#include <atomic>
#include "classes/io/device.h"
#include "task.h"
#include "globals/promise.h"

#define FILE_PUSH_DEVICE_BUFFER_SIZE (size_t)(8 * 1024 * 1024) // 8MB buffer size. TODO: Make adjustable!

namespace NX {
  struct wtf_allocator_fast_malloc_free {
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    static char *malloc(const size_type bytes) { return reinterpret_cast<char *>(WTF::fastMalloc(bytes)); }
    static void free(char *const block) { WTF::fastFree(block); }
  };

  class Nexus;
  class Context;
  namespace Classes {
    namespace IO {
      namespace Devices {
        class FilePullDevice : public virtual SeekableSourceDevice {
        public:
          FilePullDevice(const std::string &path);

          virtual ~FilePullDevice() { myStream.close(); }

        private:
          static const JSClassDefinition Class;
          static const JSStaticValue Properties[];
          static const JSStaticFunction Methods[];

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef *exception);

          static void Finalize(JSObjectRef object) {}

        public:
          static JSClassRef createClass(NX::Context *context);

          static JSObjectRef getConstructor(NX::Context *context);

          static NX::Classes::IO::Devices::FilePullDevice *FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::FilePullDevice *>(Base::FromObject(obj));
          }

          virtual std::size_t devicePosition() { return myStream.tellg(); }

          virtual std::size_t deviceRead(char *dest, std::size_t length) {
            myStream.read(dest, length);
            return myStream.gcount();
          }

          virtual bool deviceReady() const { return myStream.good(); }

          virtual std::size_t deviceSeek(std::size_t pos, Position from) {
            myStream.seekg(pos, (std::ios::seekdir) from);
            return myStream.tellg();
          }

          virtual bool eof() const { return myStream.eof(); }

          virtual std::size_t sourceSize() {
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

        class FilePushDevice : public virtual PushSourceDevice {
        public:
          FilePushDevice(NX::Scheduler *scheduler, const std::string &path);

          virtual ~FilePushDevice() { myStream.close(); }

        private:
          static const JSClassDefinition Class;
          static const JSStaticValue Properties[];
          static const JSStaticFunction Methods[];

          static void Finalize(JSObjectRef object) {}

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef *exception) {
            NX::Context *context = NX::Context::FromJsContext(ctx);
            JSClassRef fileSourceClass = createClass(context);
            try {
              if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
                throw NX::Exception("argument must be a string path");
              NX::Value path(ctx, arguments[0]);
              return JSObjectMake(ctx, fileSourceClass,
                                  dynamic_cast<NX::Classes::Base *>(
                                      new NX::Classes::IO::Devices::FilePushDevice(context->nexus()->scheduler(),
                                                                                   path.toString())));
            } catch (const std::exception &e) {
              JSWrapException(ctx, e, exception);
              return JSObjectMake(ctx, nullptr, nullptr);
            }
          }

        public:
          static JSClassRef createClass(NX::Context *context) {
            JSClassDefinition def = NX::Classes::IO::Devices::FilePushDevice::Class;
            def.parentClass = NX::Classes::IO::PushSourceDevice::createClass(context);
            return context->nexus()->defineOrGetClass(def);
          }

          static JSObjectRef getConstructor(NX::Context *context) {
            return JSObjectMakeConstructor(context->toJSContext(), createClass(context),
                                           NX::Classes::IO::Devices::FilePushDevice::Constructor);
          }

          static NX::Classes::IO::Devices::FilePushDevice *FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::FilePushDevice *>(Base::FromObject(obj));
          }

          virtual bool deviceReady() const { return myState == State::Paused; }

          virtual bool eof() const {
//            boost::recursive_mutex::scoped_lock lock(myMutex);
            return myStream.eof();
          }

          virtual JSObjectRef pause(JSContextRef ctx, JSObjectRef thisObject) {
            if (myState == Resumed) {
              myState.store(Paused);
              return myPromise;
            } else {
              NX::Context *context = NX::Context::FromJsContext(ctx);
              return myPromise = NX::Object(context->toJSContext(), NX::Globals::Promise::resolve(ctx, thisObject));
            }
          }

          virtual JSObjectRef reset(JSContextRef ctx, JSObjectRef thisObject) {
            return NX::Object(ctx, pause(ctx, thisObject)).then(
                [=](JSContextRef ctx, JSValueRef arg, JSValueRef *exception) {
//                  boost::recursive_mutex::scoped_lock lock(myMutex);
                  myStream.seekg(0, std::ios::beg);
                  return arg;
                });
          }

          virtual JSObjectRef resume(JSContextRef ctx, JSObjectRef thisObject);

          virtual State state() const { return myState; }

        private:
          NX::Scheduler *myScheduler;
          std::atomic<State> myState;
          std::atomic<NX::AbstractTask *> myTask;
          std::ifstream myStream;
          NX::Object myPromise;
//          mutable boost::recursive_mutex myMutex;
          boost::pool<wtf_allocator_fast_malloc_free> myAllocator;
        };


        class FileSinkDevice : public virtual SeekableSinkDevice {

          FileSinkDevice(const std::string &path);

          virtual ~FileSinkDevice() {
            myStream.flush();
            myStream.close();
          }

        private:
          static const JSClassDefinition Class;
          static const JSStaticValue Properties[];
          static const JSStaticFunction Methods[];

          static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef *exception);

          static void Finalize(JSObjectRef object) {}

        public:
          static JSClassRef createClass(NX::Context *context);

          static JSObjectRef getConstructor(NX::Context *context);

          static NX::Classes::IO::Devices::FileSinkDevice *FromObject(JSObjectRef obj) {
            return dynamic_cast<NX::Classes::IO::Devices::FileSinkDevice *>(Base::FromObject(obj));
          }

          virtual std::size_t devicePosition() {
//            boost::recursive_mutex::scoped_lock lock(myMutex);
            return myStream.tellp();
          }

          virtual bool deviceReady() const {
//            boost::recursive_mutex::scoped_lock lock(myMutex);
            return myStream.good();
          }

          virtual std::size_t deviceSeek(std::size_t pos, Position from) {
//            boost::recursive_mutex::scoped_lock lock(myMutex);
            myStream.seekp(pos, (std::ios::seekdir) from);
            return myStream.tellp();
          }

          virtual std::size_t recommendedWriteBufferSize() const { return 1024 * 1024; }

          virtual std::size_t maxWriteBufferSize() const { return UINT64_MAX; }

          virtual void deviceWrite(const char *buffer, std::size_t length) {
//            boost::recursive_mutex::scoped_lock lock(myMutex);
            myStream.write(buffer, length);
            myStream.flush();
          }

        private:
          std::ofstream myStream;
//          mutable boost::recursive_mutex myMutex;
        };

//         class BidirectionalFileDevice: public BidirectionalDualSeekableDevice {
//
//         };
      }
    }
  }
}

#endif // CLASSES_IO_DEVICES_FILE_H
