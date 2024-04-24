#pragma once

#include <map>
#include <napi.h>

namespace webgl
{
  class WebGLTexture : public Napi::ObjectWrap<WebGLTexture>
  {
  public:
    static void Init(Napi::Env env);
    WebGLTexture(const Napi::CallbackInfo &info);
    uint32_t GetId() const { return id_; }

  public:
    static Napi::FunctionReference *constructor;

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);
    Napi::Value GetValue(const Napi::CallbackInfo &info);

  private:
    uint32_t id_;
  };
}
