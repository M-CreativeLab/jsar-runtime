#include "./framebuffer.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLFramebuffer::constructor;
  void WebGLFramebuffer::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "WebGLFramebuffer", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(Napi::String::New(env, "WebGLFramebuffer"), func);
  }
}
