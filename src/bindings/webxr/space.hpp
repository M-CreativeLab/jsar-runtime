#pragma once

#include <napi.h>
#include <glm/glm.hpp>
#include <common/xr/types.hpp>
#include <client/xr/webxr_spaces.hpp>
#include "./common.hpp"

using namespace std;
using namespace glm;

namespace bindings
{
  template <typename ObjectType, typename HandleType = client_xr::XRSpace>
  class XRSpaceBase : public Napi::ObjectWrap<ObjectType>
  {
  public:
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle);
    static Napi::Object NewInstance(Napi::Env env, client_xr::XRSpaceSubType subType, bool isReferenceSpace = false);

  public:
    XRSpaceBase(const Napi::CallbackInfo &info);

  public:
    inline std::shared_ptr<HandleType> handle() { return handle_; }
    inline client_xr::XRSpaceSubType subType() { return handle_->subType; }
    inline bool isReferenceSpace() { return handle_->isReferenceSpace(); }
    inline glm::mat4 baseMatrix() { return handle_->baseMatrix(); }
    inline glm::mat4 inverseBaseMatrix() { return handle_->inverseBaseMatrix(); }

  protected:
    std::shared_ptr<HandleType> handle_;
  };

  class XRSpace : public XRSpaceBase<XRSpace>
  {
    friend class XRSpaceBase<XRSpace>;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRSpace> handle);
    static Napi::Object NewInstance(Napi::Env env, client_xr::XRSpaceSubType subType, mat4 baseMatrix);

  public:
    XRSpace(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRReferenceSpace : public XRSpaceBase<XRReferenceSpace, client_xr::XRReferenceSpace>
  {
    friend class XRSpaceBase<XRReferenceSpace, client_xr::XRReferenceSpace>;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRReferenceSpace> handle);
    static Napi::Object NewInstance(Napi::Env env, client_xr::XRReferenceSpaceType type);
    XRReferenceSpace(const Napi::CallbackInfo &info);

  private:
    Napi::Value GetOffsetReferenceSpace(const Napi::CallbackInfo &info);

  public:
    void onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest);

  public:
    inline client_xr::XRReferenceSpaceType referenceSpaceType() const { return handle_->referenceSpaceType(); }

  public:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRViewSpace : public XRSpaceBase<XRViewSpace, client_xr::XRViewSpace>
  {
    friend class XRSpaceBase<XRViewSpace, client_xr::XRViewSpace>;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, client_xr::XRViewSpaceType type);
    XRViewSpace(const Napi::CallbackInfo &info);

  private:
    Napi::Value EyeGetter(const Napi::CallbackInfo &info);

  public:
    inline client_xr::XREye eye() { return handle_->eye(); }
    inline glm::mat4 &projectionMatrix() { return handle_->projectionMatrix(); }

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRJointSpace : public XRSpaceBase<XRJointSpace, client_xr::XRJointSpace>
  {
    friend class XRSpaceBase<XRJointSpace, client_xr::XRJointSpace>;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRJointSpace> handle);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRInputSource> inputSource, client_xr::XRJointIndex index);

  public:
    XRJointSpace(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRTargetRayOrGripSpace : public XRSpaceBase<XRTargetRayOrGripSpace, client_xr::XRTargetRayOrGripSpace>
  {
    friend class XRSpaceBase<XRTargetRayOrGripSpace, client_xr::XRTargetRayOrGripSpace>;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRTargetRayOrGripSpace> handle);

  public:
    XRTargetRayOrGripSpace(const Napi::CallbackInfo &info);

  public:
    void onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
