#include "uniform_location.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLUniformLocation::constructor;
  void WebGLUniformLocation::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "WebGLUniformLocation",
                                     {
                                         InstanceMethod("toString", &WebGLUniformLocation::ToString),
                                         InstanceAccessor<&WebGLUniformLocation::NameGetter, &WebGLUniformLocation::NameSetter>("name"),
                                     });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
  }

  WebGLUniformLocation::WebGLUniformLocation(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLUniformLocation>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Number expected to instantiate WebGLUniformLocation")
          .ThrowAsJavaScriptException();
      return;
    }

    value_ = info[0].As<Napi::Number>().Int32Value();
  }

  Napi::Value WebGLUniformLocation::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    // Output "Program(id)"
    std::string result = "UniformLocation(" + std::to_string(value_) + ")";
    return Napi::String::New(env, result.c_str());
  }

  Napi::Value WebGLUniformLocation::NameGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, name_.c_str());
  }

  void WebGLUniformLocation::NameSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::TypeError::New(info.Env(), "Cannot set name of WebGLUniformLocation")
        .ThrowAsJavaScriptException();
  }

} // namespace webgl
