#pragma once

#include <napi.h>

namespace webgl
{
  class WebGLUniformLocation : public Napi::ObjectWrap<WebGLUniformLocation>
  {
  public:
    static void Init(Napi::Env env);
    WebGLUniformLocation(const Napi::CallbackInfo &info);
    int GetValue() const { return value_; }

  public:
    static Napi::FunctionReference *constructor;

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);

  private:
    int value_;
  };
}
