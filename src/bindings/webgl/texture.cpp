#include "texture.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLTexture::constructor;
  void WebGLTexture::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "WebGLTexture", {
      InstanceMethod("toString", &WebGLTexture::ToString),
      InstanceMethod("get", &WebGLTexture::GetValue),
    });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
  }

  WebGLTexture::WebGLTexture(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLTexture>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Number expected to instantiate WebGLTexture")
          .ThrowAsJavaScriptException();
      return;
    }

    id_ = info[0].As<Napi::Number>().Uint32Value();
  }

  Napi::Value WebGLTexture::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    // Output "Texture(id)"
    std::string result = "Texture(" + std::to_string(id_) + ")";
    return Napi::String::New(env, result.c_str());
  }

  Napi::Value WebGLTexture::GetValue(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, id_);
  }

} // namespace webgl
