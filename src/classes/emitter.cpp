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

const JSClassDefinition NX::Classes::Emitter::EventCallbackClass {
  0, kJSClassAttributeNone, "EventCallback", nullptr, nullptr, nullptr, nullptr, [](JSObjectRef thisObject) {
    delete reinterpret_cast<EventCallback*>(JSObjectGetPrivate(thisObject));
  }
};

JSClassRef NX::Classes::Emitter::createClass (NX::Context * context)
{
  JSClassDefinition def = NX::Classes::Emitter::Class;
  def.parentClass = NX::Classes::Base::createClass(context);
  return context->nexus()->defineOrGetClass (def);
}

JSObjectRef NX::Classes::Emitter::Constructor (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                               const JSValueRef arguments[], JSValueRef * exception)
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

JSValueRef NX::Classes::Emitter::addManyListener (JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, JSObjectRef callback, int count)
{
  boost::recursive_mutex::scoped_lock lock(myMutex);
  myMap[e].push_back(std::shared_ptr<Event>(new Event(e, callback, ctx, count)));
  return JSValueMakeUndefined(ctx);
}

JSValueRef NX::Classes::Emitter::addManyListener(JSGlobalContextRef ctx, JSObjectRef thisObject, const std::__cxx11::string & e, EventCallback callback, int count)
{
  NX::Context * context = NX::Context::FromJsContext(ctx);
  JSObjectRef thisObjectForBind = JSObjectMake(ctx, context->nexus()->defineOrGetClass(NX::Classes::Emitter::EventCallbackClass), new EventCallback(callback));
  JSObjectRef jsCallback = JSBindFunction(ctx, JSObjectMakeFunctionWithCallback(ctx, nullptr,
    [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
       const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      EventCallback * callback = reinterpret_cast<EventCallback*>(JSObjectGetPrivate(thisObject));
      try {
        return callback->operator()(ctx, argumentCount, arguments, exception);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
      return JSValueMakeUndefined(ctx);
    }), thisObjectForBind, 0, nullptr, nullptr);

  return addManyListener(ctx, thisObject, e, jsCallback, count);
}

JSObjectRef NX::Classes::Emitter::emit (JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string e, std::size_t
                                        argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  boost::recursive_mutex::scoped_lock lock(myMutex);
  if (myMap.find(e) != myMap.end()) {
    ProtectedArguments args(ctx, argumentCount, arguments);
    std::vector<JSValueRef> promises;
    for(auto & i: myMap[e])
    {
      JSValueProtect(ctx, thisObject);
      if (i->count > 0)
        i->count--;
      NX::Object func(i->context, i->handler);
      promises.emplace_back(NX::Globals::Promise::createPromise(ctx,
        [=](NX::Context * context, ResolveRejectHandler resolve, ResolveRejectHandler reject) {
          NX::Object funcCopy(func);
          JSValueRef exp = nullptr;
          JSValueRef val = funcCopy.call(nullptr, args.vector(), &exp);
          if (exp)
            reject(exp);
          else
            resolve(val);
          JSValueUnprotect(ctx, thisObject);
        }));
    }
    tidy(e);
    return NX::Globals::Promise::all(ctx, promises);
  } else
    return NX::Globals::Promise::resolve(ctx, JSValueMakeUndefined(ctx));
}


void NX::Classes::Emitter::emitFast (JSContextRef ctx, JSObjectRef thisObject, const std::string e, std::size_t
                                     argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  boost::recursive_mutex::scoped_lock lock(myMutex);
  if (myMap.find(e) != myMap.end()) {
    for(auto i: myMap[e])
    {
      if (i->count > 0)
        i->count--;
      lock.unlock();
      JSObjectCallAsFunction(i->context, i->handler, nullptr, argumentCount, arguments, exception);
      lock.lock();
      if (exception && *exception) {
        NX::Nexus::ReportException(ctx, *exception);
        break;
      }
    }
    tidy(e);
  }
}

void NX::Classes::Emitter::emitFastAndSchedule(JSContextRef ctx, JSObjectRef thisObject, const std::__cxx11::string e,
                                               std::size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
  boost::recursive_mutex::scoped_lock lock(myMutex);
  NX::Context * context = NX::Context::FromJsContext(ctx);
  if (myMap.find(e) != myMap.end()) {
    for(auto i: myMap[e])
    {
      if (i->count > 0)
        i->count--;
      ProtectedArguments args(ctx, argumentCount, arguments);
      NX::Object func(i->context, i->handler);
      context->nexus()->scheduler()->scheduleTask([=]() {
        NX::Object funcCopy(func);
        JSValueRef exp = nullptr;
        funcCopy.call(nullptr, args.vector(), &exp);
        if (exp)
          NX::Nexus::ReportException(context->toJSContext(), exp);
      });
      if (exception && *exception)
        break;
    }
    tidy(e);
  }
}


JSValueRef NX::Classes::Emitter::removeAllListeners (JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e)
{
  boost::recursive_mutex::scoped_lock lock(myMutex);
  myMap.erase(e);
  return JSValueMakeUndefined(ctx);
}

JSValueRef NX::Classes::Emitter::removeListener (JSGlobalContextRef ctx, JSObjectRef thisObject, const std::string & e, JSObjectRef callback)
{
  boost::recursive_mutex::scoped_lock lock(myMutex);
  if (myMap.find(e) != myMap.end()) {
    myMap[e].erase(std::remove_if(myMap[e].begin(), myMap[e].end(), [&](auto & event) {
      return JSValueIsStrictEqual(ctx, callback, event->handler);
    }), myMap[e].end());
  }
  return JSValueMakeUndefined(ctx);
}

const JSClassDefinition NX::Classes::Emitter::Class {
  0, kJSClassAttributeNone, "EventEmitter", nullptr, NX::Classes::Emitter::Properties, NX::Classes::Emitter::Methods
};

const JSStaticValue NX::Classes::Emitter::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Emitter::Methods[] {
  { "on", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        NX::Context * context = NX::Context::FromJsContext(ctx);
        if (argumentCount != 2 || JSValueGetType(ctx, arguments[0]) != kJSTypeString || JSValueGetType(ctx, arguments[1]) != kJSTypeObject)
          throw std::runtime_error("invalid arguments passed to EventEmitter.on");
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->addListener(context->toJSContext(), thisObject,
                                    NX::Value(context->toJSContext(), arguments[0]).toString(),
                                    NX::Object(context->toJSContext(), arguments[1]));
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "once", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 2 || JSValueGetType(ctx, arguments[0]) != kJSTypeString || JSValueGetType(ctx, arguments[1]) != kJSTypeObject)
          throw std::runtime_error("invalid arguments passed to EventEmitter.once");
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->addOnceListener(context->toJSContext(), thisObject, NX::Value(context->toJSContext(), arguments[0]).toString(), NX::Object(context->toJSContext(), arguments[1]));
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
            throw std::runtime_error("invalid arguments passed to EventEmitter.many");
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid EventEmitter instance");
        return emitter->addManyListener(context->toJSContext(), thisObject,
                                        NX::Value(context->toJSContext(), arguments[0]).toString(),
                                        NX::Object(context->toJSContext(), arguments[1]),
                                        NX::Value(context->toJSContext(), arguments[2]).toNumber());
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "off", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 2 || JSValueGetType(ctx, arguments[0]) != kJSTypeString || JSValueGetType(ctx, arguments[1]) != kJSTypeObject)
          throw std::runtime_error("invalid arguments passed to EventEmitter.off");
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->removeListener(context->toJSContext(), thisObject,
                                       NX::Value(context->toJSContext(), arguments[0]).toString(),
                                       NX::Object(context->toJSContext(), arguments[1]));
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "allOff", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount != 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
          throw std::runtime_error("invalid arguments passed to EventEmitter.allOff");
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->removeAllListeners(context->toJSContext(), thisObject, NX::Value(context->toJSContext(), arguments[0]).toString());
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { "emit", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      try {
        if (argumentCount < 1 || JSValueGetType(ctx, arguments[0]) != kJSTypeString)
          throw std::runtime_error("invalid arguments passed to EventEmitter.emit");
        NX::Context * context = NX::Context::FromJsContext(ctx);
        NX::Classes::Emitter * emitter = NX::Classes::Emitter::FromObject(thisObject);
        if (!emitter)
          throw std::runtime_error("invalid Emitter instance");
        return emitter->emit(context->toJSContext(), thisObject,
                             NX::Value(context->toJSContext(), arguments[0]).toString(), argumentCount - 1, arguments + 1, exception);
      } catch(const std::exception & e) {
        return JSWrapException(ctx, e, exception);
      }
    }, 0
  },
  { nullptr, nullptr, 0 }
};
