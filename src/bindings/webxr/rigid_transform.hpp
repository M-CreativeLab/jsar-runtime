#pragma once

#include <napi.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <client/xr/webxr_rigid_transform.hpp>

namespace bindings
{
  class XRRigidTransform : public Napi::ObjectWrap<XRRigidTransform>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, client_xr::XRRigidTransform handle);
    static Napi::Object NewInstance(Napi::Env env, const glm::mat4 &matrix);

  public:
    XRRigidTransform(const Napi::CallbackInfo &info);

  public:
    inline const glm::vec3 &position() const { return data_.position(); }
    inline const glm::quat &orientation() const { return data_.orientation(); }
    inline const glm::mat4 &matrix() const { return data_.matrix(); }

  private:
    Napi::Value InverseGetter(const Napi::CallbackInfo &info);

  private:
    Napi::Value CreatePositionValue(Napi::Env env);
    Napi::Value CreateOrientationValue(Napi::Env env);
    Napi::Value CreateMatrixValue(Napi::Env env);

  private:
    client_xr::XRRigidTransform data_;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
