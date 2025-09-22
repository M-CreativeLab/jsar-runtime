#include "./v8_object_wrap_base.hpp"
#include "./v8_object_wrap.hpp"

namespace scripting_base
{
  ObjectWrapBase::ObjectWrapBase(v8::Isolate *isolate)
      : current_isolate_(isolate)
      , napi_env_(nullptr)
  {
  }

  ObjectWrapBase::~ObjectWrapBase()
  {
    object_handle_.Reset();
  }

  v8::Local<v8::Object> ObjectWrapBase::getJSObject(v8::Isolate *isolate) const
  {
    return object_handle_.Get(isolate);
  }

  void ObjectWrapBase::setNapiEnv(napi_env env)
  {
    napi_env_ = env;
  }
}
