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

NX::Classes::IO::Filters::EncodingConversionFilter::EncodingConversionFilter (const std::string & fromEncoding,
                                                                     const std::string & toEncoding)
  : Filter (), myEncodingFrom(fromEncoding), myEncodingTo(toEncoding), mySource(), myTarget()
{
  UErrorCode err = U_ZERO_ERROR;
  mySource = ucnv_open(fromEncoding.c_str(), &err);
  if (U_FAILURE(err))
    throw std::runtime_error("invalid source encoding '" + myEncodingFrom + "': " + std::string(u_errorName(err)));
  myTarget = ucnv_open(toEncoding.c_str(), &err);
  if (U_FAILURE(err))
    throw std::runtime_error("invalid target encoding '" + myEncodingFrom + "': " + std::string(u_errorName(err)));
}

NX::Classes::IO::Filters::EncodingConversionFilter::~EncodingConversionFilter()
{
  ucnv_close(mySource);
  ucnv_close(myTarget);
}

std::size_t NX::Classes::IO::Filters::EncodingConversionFilter::processBuffer (const char * buffer, std::size_t length, char * dest, std::size_t outLength)
{
  UErrorCode err = U_ZERO_ERROR;
  std::size_t size = ucnv_convert(myEncodingTo.c_str(), myEncodingFrom.c_str(), dest, outLength, buffer, length, &err);
  if (U_FAILURE(err))
    throw std::runtime_error("encoding conversion error: " + std::string(u_errorName(err)));
  return size;
}
