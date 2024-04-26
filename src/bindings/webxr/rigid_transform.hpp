#pragma once

#include <napi.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace bindings {
  class XRRigidTransform : public Napi::ObjectWrap<XRRigidTransform>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, const glm::mat4 &matrix);
    XRRigidTransform(const Napi::CallbackInfo &info);

  private:
    Napi::Value PositionGetter(const Napi::CallbackInfo &info);
    Napi::Value OrientationGetter(const Napi::CallbackInfo &info);
    Napi::Value MatrixGetter(const Napi::CallbackInfo &info);
    Napi::Value InverseGetter(const Napi::CallbackInfo &info);

  public:
    glm::vec3 position;
    glm::quat orientation;
    glm::mat4 matrix;

  private:
    static Napi::FunctionReference *constructor;
  };
}
