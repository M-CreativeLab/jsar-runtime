#pragma once

#include <napi.h>

namespace webgl
{
  class WebGLShader : public Napi::ObjectWrap<WebGLShader>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WebGLShader(const Napi::CallbackInfo &info);

  private:
  };
}
