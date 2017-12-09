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

#include "object.h"
#include "exception.h"

NX::Exception::Exception(const std::string &message) : runtime_error(message) {
  myTrace = WTF::StackTrace::captureStackTrace(10);
}

NX::Exception::Exception(std::string && message) : runtime_error(message) {
  myTrace = WTF::StackTrace::captureStackTrace(10);
}

const WTF::StackTrace *NX::Exception::trace() const { return myTrace.get(); }

NX::Exception::Exception(JSContextRef ctx, JSValueRef exception):
  runtime_error(JSExceptionToString(ctx, exception))
{
  myTrace = WTF::StackTrace::captureStackTrace(10);
}

std::string NX::Exception::JSExceptionToString(JSContextRef ctx, JSValueRef exception) {
  NX::Object exp(ctx, exception);
  return exp.toString();
}

NX::Exception::Exception(const char *message) : runtime_error(message) {
  myTrace = WTF::StackTrace::captureStackTrace(10);
}

NX::Exception::Exception(const boost::system::error_code & ec): runtime_error(ec.message()) {
  myTrace = WTF::StackTrace::captureStackTrace(10);
}
