#include "value.h"
#include "object.h"
#include "exception.h"
#include "scoped_string.h"

NX::Value::Value (JSContextRef context, JSValueRef val) : myContext (context), myVal (val)
{
  JSValueProtect (context, val);
}

NX::Value::Value (JSContextRef context, JSStringRef strRef): myContext(context), myVal(nullptr)
{
  myVal = JSValueMakeString(myContext, strRef);
  JSValueProtect(context, myVal);
  JSStringRelease(strRef);
}

NX::Value::Value (JSContextRef context, const std::string & str): myContext(context), myVal(nullptr)
{
  NX::ScopedString strRef(str);
  myVal = JSValueMakeString(myContext, strRef);
  JSValueProtect(context, myVal);
}

NX::Value::Value (JSContextRef context, double value): myContext(context), myVal(nullptr)
{
  myVal = JSValueMakeNumber(myContext, value);
  JSValueProtect(context, myVal);
}


NX::Value::~Value()
{
  JSValueUnprotect (myContext, myVal);
}

std::shared_ptr<NX::Object> NX::Value::toObject()
{
  JSValueRef exception = nullptr;
  JSObjectRef val = JSValueToObject (myContext, myVal, &exception);
  if (exception)
  {
    NX::Value except (myContext, exception);
    throw NX::Exception (except.toString());
  }
  return std::shared_ptr<NX::Object>(new NX::Object (myContext, val));
}

bool NX::Value::toBoolean()
{
  return JSValueToBoolean (myContext, myVal);
}

double NX::Value::toNumber()
{
  JSValueRef exception = nullptr;
  double val = JSValueToNumber (myContext, myVal, &exception);
  if (exception)
  {
    NX::Value except (myContext, exception);
    throw NX::Exception (except.toString());
  }
  return val;
}

std::string NX::Value::toString()
{
  JSValueRef exception = nullptr;
  JSStringRef strRef = JSValueToStringCopy (myContext, myVal, &exception);
  if (exception)
  {
    NX::Object except (myContext, exception);
    throw NX::Exception (except["message"]->toString());
  }
  std::size_t len = JSStringGetMaximumUTF8CStringSize (strRef);
  std::string str (len, ' ');
  len = JSStringGetUTF8CString (strRef, &str[0], len);
  JSStringRelease (strRef);
  str.resize (len-1);
  return str;
}

std::string NX::Value::toJSON(unsigned int indent)
{
  JSValueRef exception = nullptr;
  JSStringRef strRef = JSValueCreateJSONString(myContext, myVal, indent, &exception);
  if (exception)
  {
    NX::Object except (myContext, exception);
    throw NX::Exception (except["message"]->toString());
  }
  std::size_t len = JSStringGetMaximumUTF8CStringSize (strRef);
  std::string str (len, ' ');
  len = JSStringGetUTF8CString (strRef, &str[0], len);
  JSStringRelease (strRef);
  str.resize (len);
  return str;
}
