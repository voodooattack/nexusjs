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

#include "classes/net/http/server.h"
#include "classes/net/http/connection.h"

const JSClassDefinition NX::Classes::Net::HTTP::Server::Class {
  0, kJSClassAttributeNone, "HTTPServer", nullptr, NX::Classes::Net::HTTP::Server::Properties,
  NX::Classes::Net::HTTP::Server::Methods, nullptr, NX::Classes::Net::HTTP::Server::Finalize
};

const JSStaticValue NX::Classes::Net::HTTP::Server::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Net::HTTP::Server::Methods[] {
  { nullptr, nullptr, 0 }
};

void NX::Classes::Net::HTTP::Server::handleAccept(NX::Context * context, JSObjectRef thisObject, const std::shared_ptr< boost::asio::ip::tcp::socket > & socket)
{
  beginAccept(context, thisObject);
  if (socket->is_open()) {
    NX::Object remoteEndpoint(context->toJSContext());
    try {
      remoteEndpoint.set("address", NX::Value(context->toJSContext(), socket->remote_endpoint().address().to_string()).value());
      remoteEndpoint.set("port", NX::Value(context->toJSContext(), socket->remote_endpoint().port()).value());
    } catch(const std::exception & e) {
      remoteEndpoint.set("address", JSValueMakeUndefined(context->toJSContext()));
      remoteEndpoint.set("port", JSValueMakeUndefined(context->toJSContext()));
    }
    JSObjectRef connection = NX::Classes::Net::HTTP::Connection::wrapSocket(context, socket);
    JSValueRef arguments[] {
      connection,
      remoteEndpoint
    };
    JSValueRef exception = nullptr;
    emitFastAndSchedule(context->toJSContext(), thisObject, "connection", 2, arguments, &exception);
    if (exception)
      NX::Nexus::ReportException(context->toJSContext(), exception);
  }
}
