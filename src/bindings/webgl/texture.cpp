#include "texture.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLTexture::constructor;
  void WebGLTexture::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(
      env,
      "WebGLTexture",
      {
        InstanceMethod("toString", &WebGLTexture::ToString),
        InstanceMethod("get", &WebGLTexture::GetValue),
      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
  }

  Napi::Value WebGLTexture::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    // Output "Texture(id)"
    std::string result = "Texture(" + std::to_string(handle_->id) + ")";
    return Napi::String::New(env, result.c_str());
  }

  Napi::Value WebGLTexture::GetValue(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, handle_->id);
  }

} // namespace webgl
