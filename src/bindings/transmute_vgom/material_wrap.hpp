#pragma once

#include <napi.h>
#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
  class VirtualMaterialWrap : public Napi::ObjectWrap<VirtualMaterialWrap>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    VirtualMaterialWrap(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;

  private:
    Napi::Value SetAlpha(const Napi::CallbackInfo &info);
    Napi::Value SetAlphaMode(const Napi::CallbackInfo &info);
    Napi::Value SetWireframe(const Napi::CallbackInfo &info);

    /** Standard */
    Napi::Value SetStandardDiffuseColor(const Napi::CallbackInfo &info);
    Napi::Value SetStandardSpecularColor(const Napi::CallbackInfo &info);
    Napi::Value SetStandardEmissiveColor(const Napi::CallbackInfo &info);
    Napi::Value SetStandardAmbientColor(const Napi::CallbackInfo &info);
    Napi::Value SetStandardDiffuseTexture(const Napi::CallbackInfo &info);
    Napi::Value SetStandardSpecularTexture(const Napi::CallbackInfo &info);
    Napi::Value SetStandardEmissiveTexture(const Napi::CallbackInfo &info);
    Napi::Value SetStandardAmbientTexture(const Napi::CallbackInfo &info);

    /** PBR */
    Napi::Value SetAlbedoColor(const Napi::CallbackInfo &info);
    Napi::Value SetAmbientColor(const Napi::CallbackInfo &info);
    Napi::Value SetEmissiveColor(const Napi::CallbackInfo &info);
    Napi::Value SetAlbedoTexture(const Napi::CallbackInfo &info);
    Napi::Value SetAmbientTexture(const Napi::CallbackInfo &info);
    Napi::Value SetEmissiveTexture(const Napi::CallbackInfo &info);
    Napi::Value SetMetallic(const Napi::CallbackInfo &info);
    Napi::Value SetRoughness(const Napi::CallbackInfo &info);
    Napi::Value SetSurfaceType(const Napi::CallbackInfo &info);

  public:
    int32_t id;
    std::string name;

    float alpha_;
    int32_t alpha_mode_;
    bool wireframe_;

    /** Standard */
    std::vector<float> std_diffuse_color_;
    std::vector<float> std_specular_color_;
    std::vector<float> std_emissive_color_;
    std::vector<float> std_ambient_color_;
    VirtualTexture *std_diffuse_texture_native_handle_ = nullptr;
    VirtualTexture *std_specular_texture_native_handle_ = nullptr;
    VirtualTexture *std_emissive_texture_native_handle_ = nullptr;
    VirtualTexture *std_ambient_texture_native_handle_ = nullptr;

    /** PBR */
    std::vector<float> albedo_color_;
    std::vector<float> ambient_color_;
    std::vector<float> emissive_color_;
    VirtualTexture *albedo_texture_native_handle_ = nullptr;
    VirtualTexture *ambient_texture_native_handle_ = nullptr;
    VirtualTexture *emissive_texture_native_handle_ = nullptr;
    float metallic_;
    float roughness_;
    int32_t surface_type_;
  };
} // namespace gom
