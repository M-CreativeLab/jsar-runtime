#include "./vertex_array.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLVertexArray::constructor;
  void WebGLVertexArray::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "WebGLVertexArrayObject", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(Napi::String::New(env, "WebGLVertexArrayObject"), func);
  }
} // namespace webgl
