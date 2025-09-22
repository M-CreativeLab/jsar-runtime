#pragma once

#include <node/v8.h>
#include <node/node_api.h>

namespace scripting_base
{
  class ObjectWrapBase
  {
  protected:
    ObjectWrapBase(v8::Isolate *isolate);

  public:
    virtual ~ObjectWrapBase();

  public:
    void setNapiEnv(napi_env env);
    v8::Local<v8::Object> getJSObject(v8::Isolate *) const;

  protected:
    v8::Isolate *current_isolate_;
    v8::Persistent<v8::Object> object_handle_;
    napi_env napi_env_;
  };
}
