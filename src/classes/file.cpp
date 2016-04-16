#include "module.h"
#include "value.h"
#include "object.h"

#include "classes/stream.h"
#include "classes/file.h"

JSClassRef NX::Classes::File::createClass (NX::Module * module)
{
  JSClassRef Stream = NX::Classes::Stream::createClass(module);
  JSClassDefinition FileDef = NX::Classes::File::Class;
  FileDef.parentClass = Stream;
  return module->defineOrGetClass(FileDef);
}

JSObjectRef NX::Classes::File::getConstructor (NX::Module * module)
{
  return JSObjectMakeConstructor(module->context(), NX::Classes::File::createClass(module), NX::Classes::File::Constructor);
}

JSObjectRef NX::Classes::File::Constructor (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                            const JSValueRef arguments[], JSValueRef * exception)
{
  NX::Module * module = Module::FromContext(ctx);
  JSClassRef fileClass = createClass(module);
  if (argumentCount != 2) {
    NX::Value message(ctx, "invalid arguments");
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
    return JSObjectMake(ctx, nullptr, nullptr);
  }
  try {
    /* It is important we cast to Stream here, the stream finalizer expects a Stream pointer, not a File pointer. */
    return JSObjectMake(ctx, fileClass, dynamic_cast<NX::Classes::Stream*>(new NX::Classes::File(
      module,
      NX::Value(ctx, arguments[0]).toString(),
      (std::iostream::openmode)NX::Value(ctx, arguments[1]).toNumber()
    )));
  } catch(const std::exception & e) {
    NX::Value message(ctx, e.what());
    JSValueRef args[] { message.value(), nullptr };
    *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  }
  return JSObjectMake(ctx, nullptr, nullptr);
}

NX::Classes::File::File (NX::Module * owner, const std::string & fileName,
                         std::fstream::openmode mode):
  myOwner (owner), myStream (fileName, mode)
{

}

const JSClassDefinition NX::Classes::File::Class {
  0, kJSClassAttributeNone, "File", nullptr, NX::Classes::File::Properties,
  NX::Classes::File::Methods, nullptr, NX::Classes::File::Finalize
};

const JSStaticValue NX::Classes::File::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::File::Methods[] {
  { "read", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Module * module = Module::FromContext(ctx);
      /* TODO: IMPLEMENT THIS */
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  { nullptr, nullptr, 0 }
};
