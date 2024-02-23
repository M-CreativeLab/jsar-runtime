#pragma once

#include <napi.h>

namespace webgl
{
  class WebGLProgram : public Napi::ObjectWrap<WebGLProgram>
  {
  public:
    static void Init(Napi::Env env);
    WebGLProgram(const Napi::CallbackInfo &info);
    int GetId() const { return id_; }

  public:
    static Napi::FunctionReference *constructor;

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);

  private:
    int id_;
  };
}
