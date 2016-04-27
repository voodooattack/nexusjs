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

#include "object.h"

namespace NX
{
  class Context;
  namespace Classes {
    class Emitter
    {
    private:
      static const JSClassDefinition Class;
      static const JSStaticValue Properties[];
      static const JSStaticFunction Methods[];

      static JSObjectRef Constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                     const JSValueRef arguments[], JSValueRef* exception);

      static void Finalize(JSObjectRef object) {
        delete FromObject(object);
      }

    public:
      static JSClassRef createClass(NX::Context * context);
      static JSObjectRef getConstructor(NX::Context * context);

      static NX::Classes::Emitter * FromObject(JSObjectRef object) {
        NX::Classes::Emitter * emitter = reinterpret_cast<NX::Classes::Emitter *>(JSObjectGetPrivate(object));
        return emitter;
      }

    public:
      Emitter(NX::Context * context): myContext(context) {}
      virtual ~Emitter() {}

      virtual JSValueRef addListener(JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback) {
        return addManyListener(ctx, thisObject, e, callback, -1);
      }
      virtual JSValueRef addOnceListener(JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback) {
        return addManyListener(ctx, thisObject, e, callback, 1);
      }
      virtual JSValueRef addManyListener( JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback, int count );
      virtual JSValueRef removeListener( JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback );
      virtual JSValueRef removeAllListeners( JSContextRef ctx, JSObjectRef thisObject, const std::string & e );

      /* Returns a Promise! */
      virtual JSObjectRef emit( JSContextRef ctx, JSObjectRef thisObject, const std::string e,
                                std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception );
    protected:
      virtual void tidy(const std::string & e) {
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
        Event(const std::string & name, const NX::Object & handler, int count): name(name), handler(handler), count(count) {  }
        std::string name;
        NX::Object handler;
        std::atomic_int count;
      };

      NX::Context * myContext;
      std::unordered_map<std::string, std::vector<std::shared_ptr<NX::Classes::Emitter::Event>>> myMap;
    };
  }
}

#endif // CLASSES_EMITTER_H
