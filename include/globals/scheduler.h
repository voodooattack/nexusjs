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

#ifndef GLOBALS_SCHEDULER_H
#define GLOBALS_SCHEDULER_H

#include <JavaScriptCore/API/JSContextRef.h>
#include <JavaScriptCore/API/JSObjectRef.h>
#include <JavaScriptCore/API/JSValueRef.h>

namespace NX {
  class Nexus;
  namespace Globals {
    class Scheduler
    {
      static const JSClassDefinition Class;
      static const JSStaticValue Properties[];
      static const JSStaticFunction Methods[];
      static JSValueRef Get(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception);
    public:
      static constexpr JSStaticValue GetStaticProperty() {
        return JSStaticValue { "Scheduler", &NX::Globals::Scheduler::Get, nullptr, kJSPropertyAttributeNone };
      }
    };
  }
}

#endif // GLOBALS_SCHEDULER_H
