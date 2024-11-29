#include "./object.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLObject::constructor;
  void WebGLObject::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "WebGLObject", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(Napi::String::New(env, "WebGLObject"), func);
  }
}
