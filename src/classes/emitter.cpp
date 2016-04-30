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

#include "classes/emitter.h"
#include "context.h"
#include "globals/promise.h"
#include "util.h"
#include "nexus.h"

JSClassRef NX::Classes::Emitter::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::Emitter::Class;
  return context->nexus()->defineOrGetClass (def);
}

JSObjectRef NX::Classes::Emitter::Constructor (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSClassRef emitterClass = createClass(context);
  try {
    return JSObjectMake(ctx, emitterClass, dynamic_cast<NX::Classes::Base*>(new NX::Classes::Emitter()));
  } catch (const std::exception & e) {
    JSWrapException(ctx, e, exception);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
}

JSObjectRef NX::Classes::Emitter::getConstructor (NX::Context * context)
{
  return JSObjectMakeConstructor(context->toJSContext(), createClass(context), NX::Classes::Emitter::Constructor);
}

JSValueRef NX::Classes::Emitter::addManyListener (JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback, int count)
{
  myMap[e].push_back(std::shared_ptr<Event>(new Event(e, callback, count)));
  return JSValueMakeUndefined(ctx);
}

JSObjectRef NX::Classes::Emitter::emit (JSContextRef ctx, JSObjectRef thisObject, const std::string e, std::size_t
                                        argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  if (myMap.find(e) != myMap.end()) {
    ProtectedArguments args(context->toJSContext(), argumentCount, arguments);
    std::vector<JSValueRef> promises;
    for(auto & i: myMap[e])
    {
      JSValueProtect(context->toJSContext(), thisObject);
      if (i->count > 0)
        i->count--;
      NX::Object func(context->toJSContext(), JSBindFunction(context->toJSContext(), i->handler, nullptr, args.count(), args, nullptr));
      promises.push_back(NX::Globals::Promise::createPromise(context->toJSContext(),
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
          NX::Object funcCopy(context->toJSContext(), func);
          JSValueRef exp = nullptr;
          JSValueRef val = funcCopy.call(nullptr, args.vector(), &exp);
          if (exp)
            reject(exp);
          else
            resolve(val);
          JSValueUnprotect(context->toJSContext(), thisObject);
        }));
    }
    tidy(e);
    return NX::Globals::Promise::all(context->toJSContext(), promises);
  } else
    return NX::Globals::Promise::resolve(context->toJSContext(), JSValueMakeUndefined(context->toJSContext()));
}


void NX::Classes::Emitter::emitFast (JSContextRef ctx, JSObjectRef thisObject, const std::string e, std::size_t
argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  if (myMap.find(e) != myMap.end()) {
    ProtectedArguments args(context->toJSContext(), argumentCount, arguments);
    for(auto & i: myMap[e])
    {
      if (i->count > 0)
        i->count--;
      NX::Object func(context->toJSContext(), JSBindFunction(context->toJSContext(), i->handler, nullptr, args.count(), args, nullptr));
      JSValueRef exp = nullptr;
      func.call(nullptr, args.vector(), exception);
    }
    tidy(e);
  }
}

JSValueRef NX::Classes::Emitter::removeAllListeners (JSContextRef ctx, JSObjectRef thisObject, const std::__cxx11::string & e)
{
  myMap.erase(e);
  return JSValueMakeUndefined(ctx);
}

JSValueRef NX::Classes::Emitter::removeListener (JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback)
{
  if (myMap.find(e) != myMap.end()) {
    myMap[e].erase(std::remove_if(myMap[e].begin(), myMap[e].end(), [&](auto & e) {
      return JSValueIsStrictEqual(ctx, callback.value(), e->handler.value());
    }), myMap[e].end());
  }
  return JSValueMakeUndefined(ctx);
}

const JSClassDefinition NX::Classes::Emitter::Class {
  0, kJSClassAttributeNone, "Emitter", nullptr, NX::Classes::Emitter::Properties,
  NX::Classes::Emitter::Methods, nullptr, NX::Classes::Emitter::Finalize
};

// virtual void addListener(JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback) {
//   return addManyListener(ctx, thisObject, e, callback, -1);
// }
// virtual void addOnceListener(JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback) {
//   return addManyListener(ctx, thisObject, e, callback, 1);
// }
// virtual void addManyListener( JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback, int count );
// virtual void removeListener( JSContextRef ctx, JSObjectRef thisObject, const std::string & e, const NX::Object & callback );
// virtual void removeAllListeners( JSContextRef ctx, JSObjectRef thisObject, const std::string & e );
//
// /* Returns a Promise! */
// virtual JSObjectRef emit( JSContextRef ctx, JSObjectRef thisObject, const std::string e,
//                           std::size_t argumentCount, JSValueRef arguments[], JSValueRef * exception );


const JSStaticValue NX::Classes::Emitter::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Emitter::Methods[] {
  { "on", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 2 || JSValueGetType(ctx, arguments[0]) != kJSTypeString || JSValueGetType(ctx, arguments[1]) != kJSTypeObject)
          throw std::runtime_error("invalid arguments");
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->addListener(ctx, thisObject, NX::Value(ctx, arguments[0]).toString(), NX::Object(ctx, arguments[1]));
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "once", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 2 || JSValueGetType(ctx, arguments[0]) != kJSTypeString || JSValueGetType(ctx, arguments[1]) != kJSTypeObject)
          throw std::runtime_error("invalid arguments");
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->addOnceListener(ctx, thisObject, NX::Value(ctx, arguments[0]).toString(), NX::Object(ctx, arguments[1]));
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "many", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 3 || JSValueGetType(ctx, arguments[0]) != kJSTypeString || JSValueGetType(ctx, arguments[1]) != kJSTypeObject ||
          JSValueGetType(ctx, arguments[2]) != kJSTypeNumber)
            throw std::runtime_error("invalid arguments");
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->addManyListener(ctx, thisObject, NX::Value(ctx, arguments[0]).toString(),
                                        NX::Object(ctx, arguments[1]), NX::Value(ctx, arguments[2]).toNumber());
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "off", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 2 || JSValueGetType(ctx, arguments[0]) != kJSTypeString || JSValueGetType(ctx, arguments[1]) != kJSTypeObject)
          throw std::runtime_error("invalid arguments");
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->removeListener(ctx, thisObject, NX::Value(ctx, arguments[0]).toString(), NX::Object(ctx, arguments[1]));
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "allOff", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 2 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
          throw std::runtime_error("invalid arguments");
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->removeAllListeners(ctx, thisObject, NX::Value(ctx, arguments[0]).toString());
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "emit", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
          throw std::runtime_error("invalid arguments");
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->emit(ctx, thisObject, NX::Value(ctx, arguments[0]).toString(), argumentCount - 1, arguments + 1, exception);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};
