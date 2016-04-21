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

#include "classes/io/filters/encoding.h"

#include <iconv.h>
#include <errno.h>

NX::Classes::IO::EncodingConversionFilter::EncodingConversionFilter (const NX::Object & source,
                                                                     const std::string & fromEncoding,
                                                                     const std::string & toEncoding)
  : Filter (source), myEncodingFrom(fromEncoding), myEncodingTo(toEncoding)
{
  iconv_t cd = iconv_open(myEncodingFrom.c_str(), myEncodingTo.c_str());
  if (cd = (iconv_t)-1)
    throw std::runtime_error("invalid parameters specified while converting from '" +
      myEncodingFrom + "' to '" + myEncodingTo + "'");
  iconv_close(cd);
}

std::size_t NX::Classes::IO::EncodingConversionFilter::processBuffer (char * buffer, std::size_t length, char * dest, std::size_t outLength)
{
  if (!dest) return length * 4;
  errno = 0;
  iconv_t cd = iconv_open(myEncodingFrom.c_str(), myEncodingTo.c_str());
  size_t outBytesBeforeWriting = outLength;
  if (cd = (iconv_t)-1)
    throw std::runtime_error("invalid parameters specified while converting from '" +
      myEncodingFrom + "' to '" + myEncodingTo + "'");
  size_t result = iconv(cd, &buffer, &length, &dest, &outLength);
  iconv_close(cd);
  if (result == (size_t)-1) {
    if (errno == E2BIG) {
      return length * 2;
    } else {
      throw std::runtime_error("an error occurred while converting from '" +
        myEncodingFrom + "' to '" + myEncodingTo + "'");
    }
  } else {
    return outBytesBeforeWriting - outLength;
  }
}
