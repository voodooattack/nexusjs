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

void NX::Classes::Net::HTTP::Server::handleAccept(NX::Context * context, const NX::Object & thisObject,
                                                         const std::shared_ptr< boost::asio::ip::tcp::socket > & socket,
                                                         bool continuation, const boost::system::error_code& error)
{
  if (!myThisObject) {
    myThisObject = thisObject;
  }
  if (!continuation)
    beginAccept(context, thisObject);
  if (error) {
    if (error == boost::system::errc::operation_canceled ||
        error == boost::system::errc::broken_pipe ||
        error == boost::system::errc::timed_out ||
        error == boost::system::errc::connection_aborted ||
        error == boost::system::errc::connection_reset
      )
      return;
    JSValueRef args[] { NX::Object(context->toJSContext(), error )};
    emitFastAndSchedule(context->toJSContext(), thisObject, "error", 1, args, nullptr);
  }
  if (socket->is_open()) {
    NX::Object remoteEndpoint(context->toJSContext());
    try {
      remoteEndpoint.set("address", NX::Value(context->toJSContext(), socket->remote_endpoint().address().to_string()).value());
      remoteEndpoint.set("port", NX::Value(context->toJSContext(), socket->remote_endpoint().port()).value());
    } catch(const std::exception & e) {
      remoteEndpoint.set("address", JSValueMakeUndefined(context->toJSContext()));
      remoteEndpoint.set("port", JSValueMakeUndefined(context->toJSContext()));
    }
    JSObjectRef connection = nullptr;
    auto conn = NX::Classes::Net::HTTP::Connection::wrapSocket(context, socket, this, &connection);
    if (conn) {
      JSValueRef arguments[] { connection, remoteEndpoint };
      NX::ProtectedArguments args(context->toJSContext(), 2, arguments);
      args.push_back(thisObject.value());
      NX::Object promise(context->toJSContext(), conn->start(context, connection, continuation));
      promise.then([=](JSContextRef ctx, JSValueRef value, JSValueRef *exception) {
        JSValueRef exp = nullptr;
        emitFast(context->toJSContext(), thisObject, "connection", args.size(), args, &exp);
        if (exp)
          *exception = NX::Exception(ctx, exp).toError(ctx);
        return thisObject;
      }, [=](JSContextRef ctx, JSValueRef value, JSValueRef *exception) {
        JSValueRef errorArgs[] { value };
        emitFast(context->toJSContext(), thisObject, "error", 1, errorArgs, exception);
        return JSValueMakeUndefined(ctx);
      });
    } else {
      throw NX::Exception("couldn't create connection object");
    }
  }
}
