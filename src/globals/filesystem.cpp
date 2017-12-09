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

#include "nexus.h"
#include "context.h"
#include "object.h"
#include "globals/filesystem.h"

#include <boost/filesystem.hpp>
#include <globals/promise.h>

JSValueRef NX::Globals::FileSystem::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (auto FileSystem = context->getGlobal("Nexus.FileSystem")) {
    return FileSystem;
  }
  return context->setGlobal("Nexus.FileSystem", JSObjectMake(context->toJSContext(),
                                                             context->nexus()->defineOrGetClass(NX::Globals::FileSystem::Class),
                                                             nullptr));
}

const JSClassDefinition NX::Globals::FileSystem::Class {
  0, kJSClassAttributeNone, "FileSystem", nullptr, NX::Globals::FileSystem::Properties, NX::Globals::FileSystem::Methods
};

const JSStaticValue NX::Globals::FileSystem::Properties[] {
//   { "OpenMode", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
//       NX::Context * context = Context::FromJsContext(ctx);
//       if (JSObjectRef OpenMode = context->getGlobal("Nexus.FileSystem.OpenMode"))
//         return OpenMode;
//       NX::Object modes(ctx);
//       modes.set("Read", JSValueMakeNumber(ctx, std::fstream::in));
//       modes.set("Write", JSValueMakeNumber(ctx, std::fstream::out));
//       modes.set("Binary", JSValueMakeNumber(ctx, std::fstream::binary));
//       modes.set("End", JSValueMakeNumber(ctx, std::fstream::ate));
//       modes.set("Append", JSValueMakeNumber(ctx, std::fstream::app));
//       modes.set("Truncate", JSValueMakeNumber(ctx, std::fstream::trunc));
//       return context->setGlobal("Nexus.FileSystem.OpenMode", modes.value());
//     }, nullptr, kJSPropertyAttributeNone
//   },
  { "Permissions", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (auto OpenMode = context->getGlobal("Nexus.FileSystem.Permissions"))
        return OpenMode;
      NX::Object modes(ctx);
      modes.set("AllAll",                JSValueMakeNumber(ctx, boost::filesystem::perms::all_all));
      modes.set("GroupAll",           JSValueMakeNumber(ctx, boost::filesystem::perms::group_all));
      modes.set("GroupExec",          JSValueMakeNumber(ctx, boost::filesystem::perms::group_exe));
      modes.set("GroupRead",          JSValueMakeNumber(ctx, boost::filesystem::perms::group_read));
      modes.set("GroupWrite",         JSValueMakeNumber(ctx, boost::filesystem::perms::group_write));
      modes.set("OwnerAll",           JSValueMakeNumber(ctx, boost::filesystem::perms::owner_all));
      modes.set("OwnerExec",          JSValueMakeNumber(ctx, boost::filesystem::perms::owner_exe));
      modes.set("OwnerRead",          JSValueMakeNumber(ctx, boost::filesystem::perms::owner_read));
      modes.set("OwnerWrite",         JSValueMakeNumber(ctx, boost::filesystem::perms::owner_write));
      modes.set("OthersAll",          JSValueMakeNumber(ctx, boost::filesystem::perms::others_all));
      modes.set("OthersExec",         JSValueMakeNumber(ctx, boost::filesystem::perms::others_exe));
      modes.set("OthersRead",         JSValueMakeNumber(ctx, boost::filesystem::perms::others_read));
      modes.set("OthersWrite",        JSValueMakeNumber(ctx, boost::filesystem::perms::others_write));
      modes.set("Unknown",            JSValueMakeNumber(ctx, boost::filesystem::perms::perms_not_known));
      modes.set("AddPermissions",     JSValueMakeNumber(ctx, boost::filesystem::perms::add_perms));
      modes.set("RemovePermissions",  JSValueMakeNumber(ctx, boost::filesystem::perms::remove_perms));
      modes.set("Mask",               JSValueMakeNumber(ctx, boost::filesystem::perms::perms_mask));
      modes.set("SetGID",             JSValueMakeNumber(ctx, boost::filesystem::perms::set_gid_on_exe));
      modes.set("SetUID",             JSValueMakeNumber(ctx, boost::filesystem::perms::set_uid_on_exe));
      modes.set("StickyBit",          JSValueMakeNumber(ctx, boost::filesystem::perms::sticky_bit));
      modes.set("SymlinkPermissions", JSValueMakeNumber(ctx, boost::filesystem::perms::symlink_perms));
      return context->setGlobal("Nexus.FileSystem.Permissions", modes.value());
    }, nullptr, kJSPropertyAttributeNone
  },
  { "FileType", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      if (auto OpenMode = context->getGlobal("Nexus.FileSystem.FileType"))
        return OpenMode;
      NX::Object modes(ctx);
      modes.set("Directory",          JSValueMakeNumber(ctx, boost::filesystem::file_type::directory_file));
      modes.set("Block",              JSValueMakeNumber(ctx, boost::filesystem::file_type::block_file));
      modes.set("FIFO",               JSValueMakeNumber(ctx, boost::filesystem::file_type::fifo_file));
      modes.set("NotFound",           JSValueMakeNumber(ctx, boost::filesystem::file_type::file_not_found));
      modes.set("Regular",            JSValueMakeNumber(ctx, boost::filesystem::file_type::regular_file));
      modes.set("Reparse",            JSValueMakeNumber(ctx, boost::filesystem::file_type::reparse_file));
      modes.set("Socket",             JSValueMakeNumber(ctx, boost::filesystem::file_type::socket_file));
      modes.set("Symlink",            JSValueMakeNumber(ctx, boost::filesystem::file_type::symlink_file));
      modes.set("Unknown",            JSValueMakeNumber(ctx, boost::filesystem::file_type::type_unknown));
      return context->setGlobal("Nexus.FileSystem.FileType", modes.value());
    }, nullptr, kJSPropertyAttributeNone
  },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::FileSystem::Methods[] {
  { "stat", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      try {
        std::string filePath = NX::Value(ctx, arguments[0]).toString();
        boost::filesystem::file_status stats = boost::filesystem::status(filePath);
        boost::filesystem::perms perms = stats.permissions();
        boost::filesystem::file_type type = stats.type();
        NX::Object statsObj(ctx);
        statsObj.set("type", NX::Value(ctx, type).value());
        if (stats.type() != boost::filesystem::file_type::file_not_found) {
          time_t lastMod = boost::filesystem::last_write_time(filePath);
          statsObj.set("permissions", NX::Value(ctx, perms).value());
          statsObj.set("lastModified", NX::Object(ctx, lastMod).value());
        }
        return NX::Globals::Promise::resolve(ctx, statsObj);
      } catch(const std::exception & e) {
        return NX::Globals::Promise::reject(ctx, NX::Object(context->toJSContext(), e));
      }
    }, 0
  },
  { "join", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//      NX::Context * context = Context::FromJsContext(ctx);
      try {
        if (argumentCount == 0)
          return NX::Value(ctx, "").value();
        boost::filesystem::path p(NX::Value(ctx, arguments[0]).toString());
        for(std::size_t i = 1; i < argumentCount; i++) {
          p.append(NX::Value(ctx, arguments[i]).toString());
        }
        return NX::Value(ctx, p.c_str()).value();
      } catch(const std::exception & e) {
        *exception = NX::Object(ctx, e);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { "absolute", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
//      NX::Context * context = Context::FromJsContext(ctx);
      try {
        if (argumentCount == 0)
          return NX::Value(ctx, "").value();
        boost::filesystem::path p(NX::Value(ctx, arguments[0]).toString());
        for(std::size_t i = 1; i < argumentCount; i++) {
          p.append(NX::Value(ctx, arguments[i]).toString());
        }
        return NX::Value(ctx, boost::filesystem::absolute(p).c_str()).value();
      } catch(const std::exception & e) {
        NX::Value message(ctx, e.what());
        JSValueRef args[] { message.value(), nullptr };
        *exception = JSObjectMakeError(ctx, 1, args, nullptr);
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
