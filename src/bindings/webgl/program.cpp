#include "program.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLProgram::constructor;
  void WebGLProgram::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "WebGLProgram", {
      InstanceMethod("toString", &WebGLProgram::ToString)
    });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
  }

  WebGLProgram::WebGLProgram(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLProgram>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Number expected to instantiate WebGLProgram")
          .ThrowAsJavaScriptException();
      return;
    }

    id_ = info[0].As<Napi::Number>().Int32Value();
  }

  Napi::Value WebGLProgram::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    // Output "Program(id)"
    std::string result = "Program(" + std::to_string(id_) + ")";
    return Napi::String::New(env, result.c_str());
  }

} // namespace webgl