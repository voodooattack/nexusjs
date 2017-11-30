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

#include "classes/net/http/request.h"
#include "classes/net/http/response.h"

JSObjectRef NX::Classes::Net::HTTP::Response::attach(JSContextRef ctx, JSObjectRef thisObject, JSObjectRef connection)
{
  return NX::Globals::Promise::resolve(ctx, thisObject);
}

NX::Classes::Net::HTTP::Response::Response(NX::Classes::Net::HTTP::Connection *connection) :
    HTCommon::Response(connection), myConnection(connection), myRes(), myWriter(), myStatus(200)
{
  myRes = std::make_unique<NX::Classes::Net::HTTP::Response::BeastResponse>(
    (boost::beast::http::status)myStatus, 11);
  myWriter = std::make_unique<Writer>(connection);
}

void NX::Classes::Net::HTTP::Response::deviceWrite(const char *buffer, std::size_t length) {
  boost::system::error_code ec;
  if (buffer) {
    auto & body = myRes->body()/* = boost::beast::multi_buffer()*/;
    body.commit(boost::asio::buffer_copy(
      body.prepare(length), boost::asio::const_buffers_1(buffer, length)));
  }
  if (!myHeadersSentFlag) {
    myHeadersSentFlag.store(true);
    myRes->chunked(true);
    Serializer serializer(*myRes);
    serializer.split(true);
    boost::beast::http::write_header(*myWriter, serializer, ec);
  }
  if (ec) {
    throw NX::Exception(ec.message());
  }
  if (buffer) {
    auto && constBuffers = boost::asio::const_buffers_1(buffer, length);
    boost::asio::write(*myConnection->socket(), boost::beast::http::make_chunk(constBuffers), ec);
  } else {
    boost::asio::write(*myConnection->socket(), boost::beast::http::make_chunk_last(), ec);
  }
  if (ec) {
    throw NX::Exception(ec.message());
  }
}
