#include "common/utility.hpp"
#include "./buffer.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLBuffer::constructor;
  void WebGLBuffer::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "WebGLBuffer", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(Napi::String::New(env, "WebGLBuffer"), func);
  }
}
