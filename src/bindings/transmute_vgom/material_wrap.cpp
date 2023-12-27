#include "material_wrap.hpp"
#include "texture_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualMaterialWrap::constructor;
void VirtualMaterialWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env,
      "Material",
      {
          InstanceMethod("setAlpha", &VirtualMaterialWrap::SetAlpha),
          InstanceMethod("setAlphaMode", &VirtualMaterialWrap::SetAlphaMode),
          InstanceMethod("setWireframe", &VirtualMaterialWrap::SetWireframe),

          /** Standard Setter*/
          InstanceMethod("setStandardDiffuseColor", &VirtualMaterialWrap::SetStandardDiffuseColor),
          InstanceMethod("setStandardSpecularColor", &VirtualMaterialWrap::SetStandardSpecularColor),
          InstanceMethod("setStandardEmissiveColor", &VirtualMaterialWrap::SetStandardEmissiveColor),
          InstanceMethod("setStandardAmbientColor", &VirtualMaterialWrap::SetStandardAmbientColor),
          InstanceMethod("setStandardDiffuseTexture", &VirtualMaterialWrap::SetStandardDiffuseTexture),
          InstanceMethod("setStandardSpecularTexture", &VirtualMaterialWrap::SetStandardSpecularTexture),
          InstanceMethod("setStandardEmissiveTexture", &VirtualMaterialWrap::SetStandardEmissiveTexture),
          InstanceMethod("setStandardAmbientTexture", &VirtualMaterialWrap::SetStandardAmbientTexture),

          /** PBR Setter */
          InstanceMethod("setAlbedoColor", &VirtualMaterialWrap::SetAlbedoColor),
          InstanceMethod("setAmbientColor", &VirtualMaterialWrap::SetAmbientColor),
          InstanceMethod("setEmissiveColor", &VirtualMaterialWrap::SetEmissiveColor),
          InstanceMethod("setAlbedoTexture", &VirtualMaterialWrap::SetAlbedoTexture),
          InstanceMethod("setAmbientTexture", &VirtualMaterialWrap::SetAmbientTexture),
          InstanceMethod("setEmissiveTexture", &VirtualMaterialWrap::SetEmissiveTexture),
          InstanceMethod("setMetallic", &VirtualMaterialWrap::SetMetallic),
          InstanceMethod("setRoughness", &VirtualMaterialWrap::SetRoughness),
          InstanceMethod("setSurfaceType", &VirtualMaterialWrap::SetSurfaceType),
      });

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  exports.Set("VirtualMaterial", tpl);
}

VirtualMaterialWrap::VirtualMaterialWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualMaterialWrap>(info),
                                                                           std_diffuse_color_({0.0f, 0.0f, 0.0f, 0.0f}),
                                                                           std_specular_color_({0.0f, 0.0f, 0.0f, 0.0f}),
                                                                           std_emissive_color_({0.0f, 0.0f, 0.0f, 0.0f}),
                                                                           std_ambient_color_({0.0f, 0.0f, 0.0f, 0.0f}),
                                                                           albedo_color_({0.0f, 0.0f, 0.0f, 0.0f}),
                                                                           ambient_color_({0.0f, 0.0f, 0.0f, 0.0f}),
                                                                           emissive_color_({0.0f, 0.0f, 0.0f, 0.0f})
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsObject())
  {
    Napi::TypeError::New(env, "material object is expected")
        .ThrowAsJavaScriptException();
    return;
  }

  Napi::Object materialObj = info[0].As<Napi::Object>();
  if (!materialObj.Has("uniqueId") || !materialObj.Get("uniqueId").IsNumber())
  {
    Napi::TypeError::New(env, "material object is invalid, material.uniqueId is not a number.")
        .ThrowAsJavaScriptException();
    return;
  }
  if (!materialObj.Has("name") || !materialObj.Get("name").IsString())
  {
    Napi::TypeError::New(env, "material object is invalid, material.name is not a string.")
        .ThrowAsJavaScriptException();
    return;
  }

  id = materialObj.Get("uniqueId").ToNumber().Int32Value();
  name = materialObj.Get("name").ToString().Utf8Value();
}

Napi::Value VirtualMaterialWrap::SetAlpha(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "value is expected and to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  float value = info[0].As<Napi::Number>().FloatValue();
  if (value > 1 || value < 0)
  {
    Napi::TypeError::New(env, "value should be in [0, 1]")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  this->alpha_ = value;
  info.This().As<Napi::Object>().Set("alpha", this->alpha_);
  return Napi::Boolean::New(env, true);
}

Napi::Value VirtualMaterialWrap::SetAlphaMode(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "value is expected and to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  this->alpha_mode_ = info[0].As<Napi::Number>().FloatValue();
  info.This().As<Napi::Object>().Set("alphaMode", this->alpha_);
  return Napi::Boolean::New(env, true);
}

Napi::Value VirtualMaterialWrap::SetWireframe(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsBoolean())
  {
    Napi::TypeError::New(env, "value is expected and to be a boolean")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  bool value = info[0].As<Napi::Boolean>().Value();
  this->wireframe_ = value;
  return env.Undefined();
}

#define CREATE_COLOR_SETTER(SETTER_NAME, native_handle, js_var_name)     \
  Napi::Value SETTER_NAME(const Napi::CallbackInfo &info)                \
  {                                                                      \
    Napi::Env env = info.Env();                                          \
    Napi::HandleScope scope(env);                                        \
                                                                         \
    if (info.Length() < 1 || !info[0].IsObject())                        \
    {                                                                    \
      Napi::TypeError::New(env, "value is expected and to be an object") \
          .ThrowAsJavaScriptException();                                 \
      return env.Undefined();                                            \
    }                                                                    \
                                                                         \
    Napi::Object colorObj = info[0].As<Napi::Object>();                  \
    if (colorObj.IsNull())                                               \
      return env.Undefined();                                            \
                                                                         \
    if (colorObj.Has("r") && colorObj.Get("r").IsNumber())               \
      native_handle[0] = colorObj.Get("r").ToNumber().FloatValue();      \
    if (colorObj.Has("g") && colorObj.Get("g").IsNumber())               \
      native_handle[1] = colorObj.Get("g").ToNumber().FloatValue();      \
    if (colorObj.Has("b") && colorObj.Get("b").IsNumber())               \
      native_handle[2] = colorObj.Get("b").ToNumber().FloatValue();      \
    if (colorObj.Has("a") && colorObj.Get("a").IsNumber())               \
      native_handle[3] = colorObj.Get("a").ToNumber().FloatValue();      \
                                                                         \
    info.This().As<Napi::Object>().Set(js_var_name, colorObj);           \
    return env.Undefined();                                              \
  }

#define CREATE_TEXTURE_SETTER(SETTER_NAME, native_handle, js_var_name)                                             \
  Napi::Value SETTER_NAME(const Napi::CallbackInfo &info)                                                          \
  {                                                                                                                \
    Napi::Env env = info.Env();                                                                                    \
    Napi::HandleScope scope(env);                                                                                  \
                                                                                                                   \
    if (info.Length() < 1)                                                                                         \
    {                                                                                                              \
      Napi::TypeError::New(env, "texture object is expected")                                                      \
          .ThrowAsJavaScriptException();                                                                           \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
                                                                                                                   \
    Napi::Value texValue = info[0];                                                                                \
    if (texValue.IsObject() && texValue.As<Napi::Object>().InstanceOf(VirtualTextureWrap::constructor->Value()))   \
    {                                                                                                              \
      VirtualTextureWrap *textureWrap = Napi::ObjectWrap<VirtualTextureWrap>::Unwrap(texValue.As<Napi::Object>()); \
      native_handle = textureWrap->getNativeHandle();                                                              \
      info.This().As<Napi::Object>().Set(js_var_name, texValue);                                                   \
      return Napi::Boolean::New(env, true);                                                                        \
    }                                                                                                              \
    else                                                                                                           \
    {                                                                                                              \
      Napi::TypeError::New(env, "texture object is invalid, use binding.VirtualTexture()")                         \
          .ThrowAsJavaScriptException();                                                                           \
      return env.Undefined();                                                                                      \
    }                                                                                                              \
  }

CREATE_COLOR_SETTER(VirtualMaterialWrap::SetStandardDiffuseColor, std_diffuse_color_, "standardDiffuseColor")
CREATE_COLOR_SETTER(VirtualMaterialWrap::SetStandardSpecularColor, std_specular_color_, "standardSpecularColor")
CREATE_COLOR_SETTER(VirtualMaterialWrap::SetStandardEmissiveColor, std_emissive_color_, "standardEmissiveColor")
CREATE_COLOR_SETTER(VirtualMaterialWrap::SetStandardAmbientColor, std_ambient_color_, "standardAmbientColor")

CREATE_TEXTURE_SETTER(VirtualMaterialWrap::SetStandardDiffuseTexture, std_diffuse_texture_native_handle_, "standardDiffuseTexture")
CREATE_TEXTURE_SETTER(VirtualMaterialWrap::SetStandardSpecularTexture, std_specular_texture_native_handle_, "standardSpecularTexture")
CREATE_TEXTURE_SETTER(VirtualMaterialWrap::SetStandardEmissiveTexture, std_emissive_texture_native_handle_, "standardEmissiveTexture")
CREATE_TEXTURE_SETTER(VirtualMaterialWrap::SetStandardAmbientTexture, std_ambient_texture_native_handle_, "standardAmbientTexture")

CREATE_COLOR_SETTER(VirtualMaterialWrap::SetAlbedoColor, albedo_color_, "albedoColor")
CREATE_COLOR_SETTER(VirtualMaterialWrap::SetAmbientColor, ambient_color_, "ambientColor")
CREATE_COLOR_SETTER(VirtualMaterialWrap::SetEmissiveColor, emissive_color_, "emissiveColor")

CREATE_TEXTURE_SETTER(VirtualMaterialWrap::SetAlbedoTexture, albedo_texture_native_handle_, "albedoTexture")
CREATE_TEXTURE_SETTER(VirtualMaterialWrap::SetAmbientTexture, ambient_texture_native_handle_, "ambientTexture")
CREATE_TEXTURE_SETTER(VirtualMaterialWrap::SetEmissiveTexture, emissive_texture_native_handle_, "emissiveTexture")

Napi::Value VirtualMaterialWrap::SetMetallic(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "value is expected and to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  float value = info[0].As<Napi::Number>().FloatValue();
  if (value > 1 || value < 0)
  {
    Napi::TypeError::New(env, "value should be in [0, 1]")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  this->metallic_ = value;
  info.This().As<Napi::Object>().Set("metallic", this->alpha_);
  return Napi::Boolean::New(env, true);
}

Napi::Value VirtualMaterialWrap::SetRoughness(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "value is expected and to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  float value = info[0].As<Napi::Number>().FloatValue();
  if (value > 1 || value < 0)
  {
    Napi::TypeError::New(env, "value should be in [0, 1]")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  this->roughness_ = value;
  info.This().As<Napi::Object>().Set("roughness", this->alpha_);
  return Napi::Boolean::New(env, true);
}

Napi::Value VirtualMaterialWrap::SetSurfaceType(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "value is expected and to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  this->surface_type_ = info[0].As<Napi::Number>().FloatValue();
  info.This().As<Napi::Object>().Set("surfaceType", this->alpha_);
  return Napi::Boolean::New(env, true);
}

#undef CREATE_COLOR_SETTER
#undef CREATE_TEXTURE_SETTER
