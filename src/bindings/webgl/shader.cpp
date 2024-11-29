#include "./shader.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLShader::constructor;
  void WebGLShader::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "WebGLShader", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(Napi::String::New(env, "WebGLShader"), func);
  }

  thread_local Napi::FunctionReference *WebGLShaderPrecisionFormat::constructor;
  void WebGLShaderPrecisionFormat::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "WebGLShaderPrecisionFormat", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(Napi::String::New(env, "WebGLShaderPrecisionFormat"), func);
  }

  Napi::Object WebGLShaderPrecisionFormat::NewInstance(Napi::Env env, const client_graphics::WebGLShaderPrecisionFormat &format)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = WebGLShaderPrecisionFormat::constructor->New({});
    return scope.Escape(obj).ToObject();
  }
}
