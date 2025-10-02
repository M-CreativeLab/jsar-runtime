#pragma once

#include <memory>
#include <node/v8.h>
#include <node/node_api.h>

namespace scripting_base
{
  class JSObjectHolder;

  /**
   * The base class for all JavaScript object wraps.
   */
  class BaseObject
  {
  protected:
    BaseObject(v8::Isolate *isolate);
    BaseObject(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  public:
    virtual ~BaseObject();

  public:
    v8::Local<v8::Object> getJSObject(v8::Isolate *) const;
    void setData(std::shared_ptr<JSObjectHolder>);
    void setNapiEnv(napi_env env);

  protected:
    v8::Isolate *current_isolate_;
    v8::Persistent<v8::Object> object_handle_;
    std::shared_ptr<JSObjectHolder> data_handle_;

    // @deprecated
    napi_env napi_env_;
  };
}
