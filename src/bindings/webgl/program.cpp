#include "program.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLProgram::constructor;
  void WebGLProgram::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "WebGLProgram", {InstanceMethod("toString", &WebGLProgram::ToString)});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
  }

  WebGLProgram::WebGLProgram(const Napi::CallbackInfo &info)
      : WebGLObjectBase<WebGLProgram, client_graphics::WebGLProgram>(info)
  {
  }

  Napi::Value WebGLProgram::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    // Output "Program(id)"
    std::string result = "Program(" + std::to_string(id()) + ")";
    return Napi::String::New(env, result.c_str());
  }

} // namespace webgl
