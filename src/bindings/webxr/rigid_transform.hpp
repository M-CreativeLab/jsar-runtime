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
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, const glm::mat4 &matrix);
    XRRigidTransform(const Napi::CallbackInfo &info);

  public:
    inline glm::vec3 &position() { return transformData_.position(); }
    inline glm::quat &orientation() { return transformData_.orientation(); }
    inline glm::mat4 &matrix() { return transformData_.matrix(); }

  private:
    Napi::Value InverseGetter(const Napi::CallbackInfo &info);

  private:
    Napi::Value CreatePositionValue(Napi::Env env);
    Napi::Value CreateOrientationValue(Napi::Env env);
    Napi::Value CreateMatrixValue(Napi::Env env);

  private:
    client_xr::XRRigidTransform transformData_;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
