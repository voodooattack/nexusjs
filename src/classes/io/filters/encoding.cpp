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
  : Filter (), myEncodingFrom(fromEncoding), myEncodingTo(toEncoding), mySource(), myTarget(),
    myPivotBuffer(1024, WTFAllocator<UChar>()),
    myPivotSource(myPivotBuffer.data()), myPivotTarget(myPivotBuffer.data()), myPayloadId(0)
{
  UErrorCode err = U_ZERO_ERROR;
  mySource = ucnv_open(fromEncoding.c_str(), &err);
  if (U_FAILURE(err))
    throw NX::Exception("invalid source encoding '" + myEncodingFrom + "': " + std::string(u_errorName(err)));
  myTarget = ucnv_open(toEncoding.c_str(), &err);
  if (U_FAILURE(err))
    throw NX::Exception("invalid target encoding '" + myEncodingFrom + "': " + std::string(u_errorName(err)));
}

NX::Classes::IO::Filters::EncodingConversionFilter::~EncodingConversionFilter()
{
  ucnv_close(mySource);
  ucnv_close(myTarget);
}

std::size_t NX::Classes::IO::Filters::EncodingConversionFilter::processBuffer (const char ** buffer,
                                                                        std::size_t * length,
                                                                        char **  dest,
                                                                        std::size_t * outLength)
{
  UErrorCode err = U_ZERO_ERROR;
  if (*buffer && *dest) {
    auto source = *buffer;
    auto target = *dest;
    auto outputLength = *outLength;
    auto pivotSource = myPivotSource;
    auto pivotTarget = myPivotTarget;
    ucnv_convertEx(mySource, myTarget,
                   &target, target + *outLength,  // target/target-limit
                   &source, source + *length, // source/source-limit
                   myPivotBuffer.data(), &pivotSource, &pivotTarget,
                   myPivotBuffer.data() + myPivotBuffer.size(),
                   myPayloadId == 0, false, &err);
    if (U_FAILURE(err) && err != U_BUFFER_OVERFLOW_ERROR)
      throw NX::Exception("encoding conversion error: " + std::string(u_errorName(err)));
    *length -= source - *buffer;
    *outLength -= target - *dest;
    myPivotSource = pivotSource;
    myPivotTarget = pivotTarget;
    *buffer = source;
    *dest = target;
    if (err == U_BUFFER_OVERFLOW_ERROR) {
      return outputLength;
    }
    myPayloadId++;
    return 0;
  }
  else
  {
    myPivotSource = myPivotTarget = myPivotBuffer.data();
    myPayloadId = 0;
    return 0;
  }
}
