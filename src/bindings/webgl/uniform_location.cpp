#include "./uniform_location.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLUniformLocation::constructor;
  void WebGLUniformLocation::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "WebGLUniformLocation",
                                     {
                                         InstanceMethod("toString", &WebGLUniformLocation::ToString),
                                         InstanceAccessor<&WebGLUniformLocation::NameGetter, nullptr>("name"),
                                     });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
  }

  Napi::Object WebGLUniformLocation::NewInstance(Napi::Env env, client_graphics::WebGLUniformLocation &handle)
  {
    Napi::EscapableHandleScope scope(env);
    auto handleExternal = Napi::External<client_graphics::WebGLUniformLocation>::New(env, &handle);
    Napi::Object instance = constructor->New({handleExternal});
    return scope.Escape(instance).ToObject();
  }

  WebGLUniformLocation::WebGLUniformLocation(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<WebGLUniformLocation>(info),
        handle_(std::nullopt)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      auto msg = "Failed to construct 'WebGLUniformLocation': Illegal constructor";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return;
    }

    auto handleData = info[0].As<Napi::External<client_graphics::WebGLUniformLocation>>().Data();
    handle_ = *handleData;

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.Set("_value", Napi::Number::New(env, value()));
  }

  Napi::Value WebGLUniformLocation::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    // Output "Program(id)"
    std::string result = "UniformLocation(" + std::to_string(value()) + ")";
    return Napi::String::New(env, result.c_str());
  }

  Napi::Value WebGLUniformLocation::NameGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, name());
  }
} // namespace webgl
