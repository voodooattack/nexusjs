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

#ifndef STRING_H
#define STRING_H

#include <JavaScript.h>
#include <boost/noncopyable.hpp>
#include <string>

namespace NX {
  class ScopedString: public boost::noncopyable {
  public:
    ScopedString(const char * str): myString(nullptr) { myString = JSStringCreateWithUTF8CString(str); }
    ScopedString(const std::string & str): myString(nullptr) { myString = JSStringCreateWithUTF8CString(str.c_str()); }
    ~ScopedString () { JSStringRelease(myString); }

    operator JSStringRef() { return myString; }
  private:
    JSStringRef myString;
  };
}

#endif // STRING_H
