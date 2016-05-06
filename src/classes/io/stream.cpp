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
#include "classes/io/stream.h"

#include "readable_stream.js.inc"
#include "writable_stream.js.inc"


JSObjectRef NX::Classes::IO::ReadableStream::getConstructor(NX::Context * context) {
  JSValueRef exception = nullptr;
  JSValueRef ctor = context->evaluateScript(std::string(readable_stream_js, readable_stream_js + readable_stream_js_len),
                                               nullptr, "readable_stream.js", 1, &exception);
  JSObjectRef ctorObject = JSValueToObject(context->toJSContext(), ctor, &exception);
  if (!exception)
    return ctorObject;
  else {
    NX::Nexus::ReportException(context->toJSContext(), exception);
  }
  return JSObjectMake(context->toJSContext(), nullptr, nullptr);
}

JSObjectRef NX::Classes::IO::WritableStream::getConstructor(Context * context) {
  JSValueRef exception = nullptr;
  JSValueRef ctor = context->evaluateScript(std::string(writable_stream_js, writable_stream_js + writable_stream_js_len),
                                            nullptr, "writable_stream.js", 1, &exception);
  JSObjectRef ctorObject = JSValueToObject(context->toJSContext(), ctor, &exception);
  if (!exception)
    return ctorObject;
  else
    NX::Nexus::ReportException(context->toJSContext(), exception);
  return JSObjectMake(context->toJSContext(), nullptr, nullptr);
}


