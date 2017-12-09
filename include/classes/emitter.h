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

#ifndef CLASSES_EMITTER_H
#define CLASSES_EMITTER_H

#include <JavaScript.h>
#include <string>
#include <atomic>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <utility>

#include "object.h"
#include "classes/base.h"

#include <wtf/FastMalloc.h>

namespace NX
{
  class Context;
  namespace Classes {
    class Emitter: public virtual NX::Classes::Base
    {
    private:
      static const JSClassDefinition Class;
      static const JSStaticValue Properties[];
      static const JSStaticFunction Methods[];

      static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                     const JSValueRef arguments[], JSValueRef* exception);

    public:
      static JSClassRef createClass(NX::Context * context);
      static JSObjectRef getConstructor(NX::Context * context);

      static NX::Classes::Emitter * FromObject(JSObjectRef object) {
        return dynamic_cast<NX::Classes::Emitter *>(Base::FromObject(object));
      }

    public:
      Emitter(): myMap(), myTidyLock() {}

      ~Emitter() override = default;

      typedef std::function<JSValueRef(JSContextRef, std::size_t argumentCount, const JSValueRef arguments[], JSValueRef *)> EventCallback;

      virtual JSValueRef addListener(JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, EventCallback callback) {
        return addManyListener(ctx, thisObject, e, std::move(callback), -1);
      }
      virtual JSValueRef addOnceListener(JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, EventCallback callback) {
        return addManyListener(ctx, thisObject, e, std::move(callback), 1);
      }
      virtual JSValueRef addManyListener( JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, EventCallback callback, int count );

      virtual JSValueRef addListener(JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, JSObjectRef callback) {
        return addManyListener(ctx, thisObject, e, callback, -1);
      }
      virtual JSValueRef addOnceListener(JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, JSObjectRef callback) {
        return addManyListener(ctx, thisObject, e, callback, 1);
      }
      virtual JSValueRef addManyListener( JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, JSObjectRef callback, int count );
      virtual JSValueRef removeListener( JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, JSObjectRef callback );
      virtual JSValueRef removeAllListeners( JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e );

      /* Returns a Promise! (slow) */
      virtual JSObjectRef emit( JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e,
                                std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception );

      /* Fast version that returns schedules the calls and returns the tasks */
      virtual NX::TaskGroup emitFastAndSchedule( JSContextRef ctx, JSObjectRef thisObject, const std::string & e,
                             std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception );

      /* Faster version that returns nothing */
      virtual void emitFast( JSContextRef ctx, JSObjectRef thisObject, const std::string & e,
                             std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception );


      static const JSClassDefinition EventCallbackClass;

    protected:
      virtual void tidy(JSContextRef ctx, const std::string & e);

    WTF_MAKE_FAST_ALLOCATED;

    private:
      struct Event: public boost::noncopyable {
        Event(std::string name, NX::Object && handler, int count):
          name(std::move(name)), handler(handler), count(count) { }
        std::string name;
        NX::Object handler;
        std::atomic_int count;
        WTF_MAKE_FAST_ALLOCATED;
      };
      std::unordered_map<std::string, std::vector<std::shared_ptr<NX::Classes::Emitter::Event>>> myMap;
      boost::shared_mutex myTidyLock;
    };
  }
}

#endif // CLASSES_EMITTER_H
