#include "texture_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualTextureWrap::constructor;
void VirtualTextureWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env,
      "Texture",
      {
          InstanceAccessor<&VirtualTextureWrap::GetGuid, &VirtualTextureWrap::SetGuid>("guid"),
          InstanceAccessor<&VirtualTextureWrap::GetHeight, &VirtualTextureWrap::SetHeight>("height"),
          InstanceAccessor<&VirtualTextureWrap::GetWidth, &VirtualTextureWrap::SetWidth>("width"),
          InstanceAccessor<&VirtualTextureWrap::GetInvertY, &VirtualTextureWrap::SetInvertY>("invertY"),
          InstanceMethod("setPixels", &VirtualTextureWrap::SetPixels),
          InstanceMethod("setSourceUrl", &VirtualTextureWrap::SetSourceUrl),
      });

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  exports.Set("VirtualTexture", tpl);
}

VirtualTextureWrap::VirtualTextureWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualTextureWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "texture name is expected and to be a string")
        .ThrowAsJavaScriptException();
    return;
  }

  native_handle_ = new VirtualTexture();
  native_handle_->set_name(info[0].ToString().Utf8Value());
}

Napi::Value VirtualTextureWrap::GetGuid(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  return Napi::Number::New(env, native_handle_->guid());
}

void VirtualTextureWrap::SetGuid(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (!value.IsNumber())
  {
    Napi::TypeError::New(env, "guid is expected and to be a number")
        .ThrowAsJavaScriptException();
    return;
  }

  native_handle_->set_guid(value.ToNumber().Uint32Value());
}

Napi::Value VirtualTextureWrap::GetHeight(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  return Napi::Number::New(env, native_handle_->height());
}

void VirtualTextureWrap::SetHeight(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (!value.IsNumber())
  {
    Napi::TypeError::New(env, "height is expected and to be a number")
        .ThrowAsJavaScriptException();
    return;
  }

  native_handle_->set_height(value.ToNumber().Int32Value());
}

Napi::Value VirtualTextureWrap::GetWidth(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  return Napi::Number::New(env, native_handle_->width());
}

void VirtualTextureWrap::SetWidth(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (!value.IsNumber())
  {
    Napi::TypeError::New(env, "width is expected and to be a number")
        .ThrowAsJavaScriptException();
    return;
  }

  native_handle_->set_width(value.ToNumber().Int32Value());
}

Napi::Value VirtualTextureWrap::GetInvertY(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  return Napi::Boolean::New(env, native_handle_->is_invert_y());
}

void VirtualTextureWrap::SetInvertY(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  native_handle_->set_is_invert_y(value.ToBoolean().Value());
}

Napi::Value VirtualTextureWrap::SetPixels(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsTypedArray())
  {
    Napi::TypeError::New(env, "pixels is expected and to be a typed array")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  auto pixels = info[0].As<Napi::Uint8Array>();
  size_t arraySize = pixels.ElementLength();
  const uint8_t *data = reinterpret_cast<const uint8_t *>(pixels.Data());

  std::string byteData;
  byteData.resize(arraySize * sizeof(uint8_t));
  memcpy(&byteData[0], data, arraySize * sizeof(uint8_t));
  native_handle_->set_pixels(byteData);

  return env.Undefined();
}

Napi::Value VirtualTextureWrap::SetSourceUrl(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "url is expected and to be a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::String urlString = info[0].ToString();
  native_handle_->set_url(urlString.Utf8Value());
  return env.Undefined();
}

VirtualTexture *VirtualTextureWrap::getNativeHandle()
{
  return native_handle_;
}
