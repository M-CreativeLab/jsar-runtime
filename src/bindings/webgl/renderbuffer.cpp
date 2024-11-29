#include "./renderbuffer.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLRenderbuffer::constructor;
  void WebGLRenderbuffer::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "WebGLRenderbuffer", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(Napi::String::New(env, "WebGLRenderbuffer"), func);
  }
}
