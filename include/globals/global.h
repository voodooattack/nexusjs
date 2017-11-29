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

#ifndef GLOBALS_GLOBAL_H
#define GLOBALS_GLOBAL_H

#include <JavaScriptCore/API/JSValueRef.h>
#include <JavaScriptCore/API/JSContextRef.h>
#include <JavaScriptCore/API/JSObjectRef.h>

namespace NX {
  class Nexus;

  class Global
  {
  private:
    ~Global() = default;
  public:
    static JSValueRef NexusGet (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception);
    static constexpr JSClassDefinition InitGlobalClass();

    static const JSStaticFunction GlobalFunctions[];
    static const JSStaticValue GlobalProperties[];
    static const JSClassDefinition GlobalClass;

    static const JSStaticFunction NexusFunctions[];
    static const JSStaticValue NexusProperties[];
    static const JSClassDefinition NexusClass;
  };
}

#endif // GLOBALS_GLOBAL_H
