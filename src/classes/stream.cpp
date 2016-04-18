#include "context.h"
#include "classes/stream.h"

JSClassRef NX::Classes::Stream::createClass (NX::Context * context)
{
  return context->defineOrGetClass(NX::Classes::Stream::Class);
}

const JSClassDefinition NX::Classes::Stream::Class {
  0, kJSClassAttributeNone, "Stream", nullptr, NX::Classes::Stream::Properties,
  NX::Classes::Stream::Methods, nullptr, NX::Classes::Stream::Finalize
};

const JSStaticValue NX::Classes::Stream::Properties[] {
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Classes::Stream::Methods[] {
  { nullptr, nullptr, 0 }
};
