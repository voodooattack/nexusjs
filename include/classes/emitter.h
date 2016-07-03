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
#include <boost/thread/recursive_mutex.hpp>

#include "object.h"
#include "classes/base.h"

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
      Emitter(): myMap() {}
      virtual ~Emitter() {}

      typedef std::function<JSValueRef(JSContextRef, std::size_t argumentCount, const JSValueRef arguments[], JSValueRef *)> EventCallback;

      virtual JSValueRef addListener(JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, EventCallback callback) {
        return addManyListener(ctx, thisObject, e, callback, -1);
      }
      virtual JSValueRef addOnceListener(JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, EventCallback callback) {
        return addManyListener(ctx, thisObject, e, callback, 1);
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

      /* Returns a Promise! */
      virtual JSObjectRef emit( JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string e,
                                std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception );

      /* Faster version that returns nothing */
      virtual void emitFast( JSContextRef ctx, JSObjectRef thisObject, const std::string e,
                                std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception );

      /* Faster version that returns nothing and schedules the call */
      virtual void emitFastAndSchedule( JSContextRef ctx, JSObjectRef thisObject, const std::string e,
                             std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception );


      static const JSClassDefinition EventCallbackClass;

    protected:
      virtual void tidy(const std::string & e) {
        boost::recursive_mutex::scoped_lock lock(myMutex);
        if (myMap.find(e) != myMap.end()) {
          for(auto i = myMap[e].begin(); i != myMap[e].end();)
          {
            if ((*i)->count == 0) {
              myMap[e].erase(i++);
              continue;
            }
            i++;
          }
          if (myMap[e].empty())
            myMap.erase(e);
        }
      }

    private:
      struct Event: public boost::noncopyable {
        Event(const std::string & name, JSObjectRef handler, JSGlobalContextRef context, int count):
          name(name), handler(handler), context(context), count(count)
        {
          JSGlobalContextRetain(context);
          JSValueProtect(context, handler);
        }
        ~Event() {
          JSValueUnprotect(context, handler);
          JSGlobalContextRelease(context);
        }
        std::string name;
        JSObjectRef handler;
        JSGlobalContextRef context;
        std::atomic_int count;
      };
      boost::recursive_mutex myMutex;
      std::unordered_map<std::string, std::vector<std::shared_ptr<NX::Classes::Emitter::Event>>> myMap;
    };
  }
}

#endif // CLASSES_EMITTER_H
