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

#ifndef CLASSES_IO_FILTERS_ENCODING_H
#define CLASSES_IO_FILTERS_ENCODING_H

#include "classes/io/filter.h"

namespace NX {
  namespace Classes {
    namespace IO {
      class EncodingConversionFilter: public NX::Classes::IO::Filter
      {
      public:
        EncodingConversionFilter (const NX::Object & source,
                                  const std::string & fromEncoding,
                                  const std::string & toEncoding);
        virtual ~EncodingConversionFilter() {}

        virtual std::size_t processBuffer(const char * buffer, std::size_t length, char * dest = nullptr, std::size_t outLength = 0);

      protected:
        std::string myEncodingFrom, myEncodingTo;
      };
    }
  }
}

#endif // CLASSES_IO_FILTERS_ENCODING_H
