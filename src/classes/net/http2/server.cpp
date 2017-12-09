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

#include "classes/net/http2/server.h"

const JSClassDefinition NX::Classes::Net::HTTP2::Server::Class {
  0, kJSClassAttributeNone, "HTTP2Server", nullptr, NX::Classes::Net::HTTP2::Server::Properties,
  NX::Classes::Net::HTTP2::Server::Methods, nullptr, NX::Classes::Net::HTTP2::Server::Finalize
};

const JSStaticValue NX::Classes::Net::HTTP2::Server::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Net::HTTP2::Server::Methods[] {
  { nullptr, nullptr, 0 }
};

void NX::Classes::Net::HTTP2::Server::handleAccept(NX::Context * context, const NX::Object & thisObject, const std::shared_ptr< boost::asio::ip::tcp::socket > & socket)
{
  beginAccept(context, thisObject);
}
