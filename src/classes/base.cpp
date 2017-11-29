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

#include "context.h"
#include "classes/base.h"

JSClassDefinition NX::Classes::Base::Class {
  0, kJSClassAttributeNone, "NativeObject", nullptr, nullptr, nullptr, nullptr, [](JSObjectRef object) {
    delete reinterpret_cast<NX::Classes::Base*>(JSObjectGetPrivate(object));
  }
};

JSClassRef NX::Classes::Base::createClass(NX::Context *context) {
  return context->nexus()->defineOrGetClass(NX::Classes::Base::Class);
}
