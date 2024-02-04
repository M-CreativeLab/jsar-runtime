#pragma once

#include <napi.h>

namespace webgl
{
  class WebGLProgram : public Napi::ObjectWrap<WebGLProgram>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WebGLProgram(const Napi::CallbackInfo &info);

  private:
  };
}
