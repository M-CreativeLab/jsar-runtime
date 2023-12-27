#include <napi.h>

#include "proto/transmute_vgom.pb.h"
#include "go_material_sync_change_wrap.hpp"
#include "material_wrap.hpp"

using namespace gom;

Napi::FunctionReference *GameObjectMaterialSyncChangeWrap::constructor;
void GameObjectMaterialSyncChangeWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env, "MaterialSyncChange", {});

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
}

GameObjectMaterialSyncChangeWrap::GameObjectMaterialSyncChangeWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<GameObjectMaterialSyncChangeWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsObject())
  {
    Napi::TypeError::New(env, "Guid(String), Type(String) and Material(Object) are expected and be valid")
        .ThrowAsJavaScriptException();
    return;
  }

  if (info.Length() == 4 && info[3].IsExternal())
  {
    native_handle_ = info[3].As<Napi::External<MaterialSyncChangeOnGameObject>>().Data();
  }
  else
  {
    Napi::TypeError::New(env, "PropertyChange cannot be created in JavaScript")
        .ThrowAsJavaScriptException();
    return;
  }

  Napi::String guid = info[0].ToString();
  Napi::String type = info[1].ToString();
  native_handle_->set_target_object_guid(guid.Utf8Value().c_str());

  Napi::Object materialObj = info[2].ToObject();
  if (materialObj.InstanceOf(VirtualMaterialWrap::constructor->Value()))
  {
    VirtualMaterialWrap *materialWrap = Napi::ObjectWrap<VirtualMaterialWrap>::Unwrap(materialObj);
    native_handle_->set_material_id(materialWrap->id);
    native_handle_->set_material_name(materialWrap->name.c_str());
    native_handle_->set_material_custom_type(type.Utf8Value().c_str());

    /** Common */
    native_handle_->set_material_alpha(materialWrap->alpha_);
    native_handle_->set_material_alpha_mode(materialWrap->alpha_mode_);
    native_handle_->set_material_surface_type(materialWrap->surface_type_);
    native_handle_->set_material_wireframe(materialWrap->wireframe_);

    /** Standard */
    // setup for colors
    for (auto &channelVal : materialWrap->std_diffuse_color_)
      native_handle_->add_material_std_diffuse_color(channelVal);
    for (auto &channelVal : materialWrap->std_specular_color_)
      native_handle_->add_material_std_specular_color(channelVal);
    for (auto &channelVal : materialWrap->std_emissive_color_)
      native_handle_->add_material_std_emissive_color(channelVal);
    for (auto &channelVal : materialWrap->std_ambient_color_)
      native_handle_->add_material_std_ambient_color(channelVal);
    // setup for textures
    if (materialWrap->std_diffuse_texture_native_handle_ != nullptr)
      native_handle_->set_allocated_material_std_diffuse_texture(materialWrap->std_diffuse_texture_native_handle_);
    if (materialWrap->std_specular_texture_native_handle_ != nullptr)
      native_handle_->set_allocated_material_std_specular_texture(materialWrap->std_specular_texture_native_handle_);
    if (materialWrap->std_emissive_texture_native_handle_ != nullptr)
      native_handle_->set_allocated_material_std_emissive_texture(materialWrap->std_emissive_texture_native_handle_);
    if (materialWrap->std_ambient_texture_native_handle_ != nullptr)
      native_handle_->set_allocated_material_std_ambient_texture(materialWrap->std_ambient_texture_native_handle_);

    /** PBR */
    // setup for colors
    for (auto &channelVal : materialWrap->albedo_color_)
      native_handle_->add_material_pbr_albedo_color(channelVal);
    for (auto &channelVal : materialWrap->ambient_color_)
      native_handle_->add_material_pbr_ambient_color(channelVal);
    for (auto &channelVal : materialWrap->emissive_color_)
      native_handle_->add_material_pbr_emissive_color(channelVal);
    // setup for textures
    if (materialWrap->albedo_texture_native_handle_ != nullptr)
      native_handle_->set_allocated_material_pbr_albedo_texture(materialWrap->albedo_texture_native_handle_);
    if (materialWrap->ambient_texture_native_handle_ != nullptr)
      native_handle_->set_allocated_material_pbr_ambient_texture(materialWrap->ambient_texture_native_handle_);
    if (materialWrap->emissive_texture_native_handle_ != nullptr)
      native_handle_->set_allocated_material_pbr_emissive_texture(materialWrap->emissive_texture_native_handle_);
    // setup for properties
    native_handle_->set_material_pbr_metallic(materialWrap->metallic_);
    native_handle_->set_material_pbr_roughness(materialWrap->roughness_);
  }
  else
  {
    Napi::TypeError::New(env, "material object is invalid, use binding.VirtualMaterial()")
        .ThrowAsJavaScriptException();
    return;
  }
}
