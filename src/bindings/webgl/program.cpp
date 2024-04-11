#include "program.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLProgram::constructor;
  void WebGLProgram::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "WebGLProgram",
                                     {InstanceMethod("toString", &WebGLProgram::ToString)});
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

  void WebGLProgram::SetAttribLocation(const std::string &name, int location)
  {
    attribLocations_[name] = location;
  }

  bool WebGLProgram::HasAttribLocation(const std::string &name)
  {
    return attribLocations_.find(name) != attribLocations_.end();
  }

  int WebGLProgram::GetAttribLocation(const std::string &name)
  {
    return attribLocations_[name];
  }

  void WebGLProgram::SetUniformLocation(const std::string &name, int location)
  {
    uniformLocations_[name] = location;
  }

  bool WebGLProgram::HasUniformLocation(const std::string &name)
  {
    return uniformLocations_.find(name) != uniformLocations_.end();
  }

  int WebGLProgram::GetUniformLocation(const std::string &name)
  {
    return uniformLocations_[name];
  }

  void WebGLProgram::SetUniformBlockIndex(const std::string &name, int index)
  {
    uniformBlockIndices_[name] = index;
  }

  bool WebGLProgram::HasUniformBlockIndex(const std::string &name)
  {
    return uniformBlockIndices_.find(name) != uniformBlockIndices_.end();
  }

  int WebGLProgram::GetUniformBlockIndex(const std::string &name)
  {
    return uniformBlockIndices_[name];
  }

  Napi::Value WebGLProgram::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    // Output "Program(id)"
    std::string result = "Program(" + std::to_string(id_) + ")";
    return Napi::String::New(env, result.c_str());
  }

} // namespace webgl
