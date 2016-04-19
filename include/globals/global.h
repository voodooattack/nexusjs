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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <JavaScript.h>

namespace NX {
  class Nexus;
  class Global
  {
  private:
    Global();
    ~Global();
  public:
    static JSValueRef NexusGet (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception);
    static constexpr JSClassDefinition InitGlobalClass();

    static JSStaticFunction GlobalFunctions[];
    static JSStaticValue GlobalProperties[];
    static JSClassDefinition GlobalClass;

    static JSStaticFunction NexusFunctions[];
    static JSStaticValue NexusProperties[];
    static JSClassDefinition NexusClass;
  };
}

#endif // GLOBAL_H
