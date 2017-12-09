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

#include <JavaScriptCore/API/JSObjectRef.h>
#include <fstream>
#include <atomic>
#include "classes/io/device.h"
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "task.h"
#include "globals/promise.h"

#define FILE_PUSH_DEVICE_BUFFER_SIZE (size_t)(8 * 1024 * 1024) // 8MB buffer size. TODO: Make it adjustable!

namespace NX {

  class Nexus;
  class Context;
  namespace Classes {
    namespace IO {
      namespace Devices {
        class FilePullDevice : public virtual SeekableSourceDevice {
        public:
          explicit FilePullDevice(const std::string &path);

          ~FilePullDevice() override { myStream.close(); }

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

          std::size_t devicePosition() override { return static_cast<size_t>(myStream.tellg()); }

          std::size_t deviceRead(char *dest, std::size_t length) override {
            myStream.read(dest, length);
            if (!myStream.good())
              myError = boost::system::errc::make_error_code(boost::system::errc::io_error);
            return static_cast<size_t>(myStream.gcount());
          }

          bool deviceReady() const override { return myStream.good(); }
          bool deviceOpen() const override { return myStream.is_open(); }

          void deviceClose() override { myStream.close(); }

          const boost::system::error_code & deviceError() const override { return myError; }

          std::size_t deviceSeek(std::size_t pos, Position from) override {
            myStream.seekg(pos, (std::ios::seekdir) from);
            if (!myStream.good())
              myError = boost::system::errc::make_error_code(boost::system::errc::invalid_seek);
            return static_cast<size_t>(myStream.tellg());
          }

          bool eof() const override { return myStream.eof(); }

          std::size_t sourceSize() override {
            std::streampos current = myStream.tellg();
            myStream.seekg(0, std::ios::beg);
            if (!myStream.good()) {
              myError = boost::system::errc::make_error_code(boost::system::errc::invalid_seek);
              return 0;
            }
            std::streampos beg = myStream.tellg();
            myStream.seekg(0, std::ios::end);
            if (!myStream.good()) {
              myError = boost::system::errc::make_error_code(boost::system::errc::invalid_seek);
              return 0;
            }
            std::streampos end = myStream.tellg();
            myStream.seekg(current, std::ios::beg);
            if (!myStream.good()) {
              myError = boost::system::errc::make_error_code(boost::system::errc::invalid_seek);
              return 0;
            }
            return static_cast<size_t>(end - beg);
          }

          std::size_t deviceBytesAvailable() override {
            return sourceSize() - myStream.tellg();
          }

        private:
          std::ifstream myStream;
          boost::system::error_code myError;
        };

        class FilePushDevice : public virtual PushSourceDevice {
        public:
          FilePushDevice(NX::Scheduler *scheduler, const std::string &path);

          ~FilePushDevice() override { myStream.close(); }

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
              if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString) {
                *exception = NX::Object(ctx, NX::Exception("path must be a string"));
                return nullptr;
              }
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

          bool deviceReady() const override { return myState == State::Paused && myStream.good() && !myError; }
          bool deviceOpen() const override  { return myStream.is_open(); }
          void deviceClose() override { myStream.close(); }

          const boost::system::error_code & deviceError() const override  { return myError; }

          bool eof() const override {
            return myStream.eof();
          }

          JSObjectRef pause(JSContextRef ctx, JSObjectRef thisObject) override {
            if (myState == Resumed) {
              myState.store(Paused);
              return myPromise;
            } else {
              NX::Context *context = NX::Context::FromJsContext(ctx);
              return myPromise = NX::Object(context->toJSContext(), NX::Globals::Promise::resolve(ctx, thisObject));
            }
          }

          JSObjectRef reset(JSContextRef ctx, JSObjectRef thisObject) override {
            myStream.seekg(0, std::ios_base::beg);
            return NX::Globals::Promise::resolve(ctx, thisObject);
          }

          JSObjectRef resume(JSContextRef ctx, JSObjectRef thisObject) override;

          State state() const override { return myState; }

        private:
          NX::Scheduler *myScheduler;
          std::string myPath;
          std::atomic<State> myState;
          std::atomic<NX::AbstractTask *> myTask;
          std::ifstream myStream;
          NX::Object myPromise;
          boost::system::error_code myError;
        };

        class FileSinkDevice : public virtual SeekableSinkDevice {

          explicit FileSinkDevice(const std::string &path);
          explicit FileSinkDevice(int fd, bool close);

          ~FileSinkDevice() override {
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

          std::size_t devicePosition() override {
            return static_cast<size_t>(myStream.tellp());
          }

          bool deviceReady() const override {
            return myStream.good();
          }

          const boost::system::error_code & deviceError() const override  {
            return myError;
          }
          bool deviceOpen() const override { return myStream.is_open(); }
          void deviceClose() override { myStream.close(); }

          std::size_t deviceSeek(std::size_t pos, Position from) override {
            myStream.seekp(pos, (std::ios_base::seekdir)from);
            return static_cast<size_t>(myStream.tellp());
          }

          std::size_t recommendedWriteBufferSize() const override { return 8 * 1024 * 1024; }
          std::size_t maxWriteBufferSize() const override { return 8 * 1024 * 1024; }

          std::size_t deviceWrite(const char *buffer, std::size_t length) override {
            if (buffer && length) {
              myStream.write(buffer, length);
              myStream.flush();
              return length;
            } else
              myStream.flush();
            return 0;
          }

        private:
          boost::iostreams::stream<boost::iostreams::file_descriptor> myStream;
          boost::system::error_code myError;
        };


        class BidirectionalFileDevice: public BidirectionalDualSeekableDevice {
          explicit BidirectionalFileDevice(const std::string &path) {}
          explicit BidirectionalFileDevice(int fd) {}
        };
      }
    }
  }
}

#endif // CLASSES_IO_DEVICES_FILE_H
