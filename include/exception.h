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

#ifndef NX_EXCEPTION_H
#define NX_EXCEPTION_H

#include <memory>
#include <stdexcept>
#include <wtf/FastMalloc.h>
#include <wtf/StackTrace.h>
#include <JavaScriptCore/API/JSValueRef.h>
#include <JavaScriptCore/API/JSObjectRef.h>

#include "scoped_string.h"

namespace WTF {
  class StackTrace;
}

namespace NX
{
  class Exception: public std::runtime_error {
  public:
    explicit Exception(const char * message);
    explicit Exception(const std::string & message);
    explicit Exception(std::string && message);
    explicit Exception(const boost::system::error_code & ec);
    explicit Exception(JSContextRef ctx, JSValueRef exception);

    const WTF::StackTrace * trace() const;

    JSObjectRef toError(JSContextRef ctx) {
      JSValueRef args[] {
        JSValueMakeString(ctx, NX::ScopedString(what()))
      };;
      return JSObjectMakeError(ctx, 1, args, nullptr);
    }

  protected:
    static std::string JSExceptionToString(JSContextRef ctx, JSValueRef exception);

    std::unique_ptr<WTF::StackTrace> myTrace;
  };

}

#endif