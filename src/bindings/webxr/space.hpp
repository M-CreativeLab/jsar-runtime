#pragma once

#include <napi.h>
#include <glm/glm.hpp>
#include "common/xr/types.hpp"
#include "./common.hpp"

using namespace std;
using namespace glm;

namespace bindings
{
#define XRSPACE_RELATIVE_TRANSFORM(space, baseSpace) baseSpace->getInverseBaseMatrix() * space->baseMatrix

  template <typename T>
  class XRSpaceBase : public Napi::ObjectWrap<T>
  {
  public:
    XRSpaceBase(const Napi::CallbackInfo &info, XRSpaceSubType subType, bool isReferenceSpace = false);

  public:
    void onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest);
    void ensurePoseUpdated(uint32_t frameId, XRSession *session, xr::TrXRFrameRequest *frameRequest);
    mat4 &getInverseBaseMatrix();

  public:
    XRSpaceSubType subType;
    int32_t lastFrameId;
    mat4 baseMatrix;
    bool isReferenceSpace;

  private:
    mat4 mInverseMatrixCache;
    bool mIsInverseMatrixDirty = true;
  };

  class XRSpace : public XRSpaceBase<XRSpace>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRSpaceSubType subType, mat4 baseMatrix);
    XRSpace(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRReferenceSpace : public XRSpaceBase<XRReferenceSpace>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRReferenceSpaceType type);
    XRReferenceSpace(const Napi::CallbackInfo &info);

  private:
    Napi::Value GetOffsetReferenceSpace(const Napi::CallbackInfo &info);

  public:
    void onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest);

  public:
    XRReferenceSpaceType referenceSpaceType;
    mat4 offsetMatrix;

  public:
    static Napi::FunctionReference *constructor;
  };

  class XRViewSpace : public XRSpaceBase<XRViewSpace>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, XRViewSpaceType type);
    XRViewSpace(const Napi::CallbackInfo &info);

  private:
    Napi::Value EyeGetter(const Napi::CallbackInfo &info);

  public:
    void onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest);
    XREye getEye();
    glm::mat4 &getProjectionMatrix();

  private:
    XRViewSpaceType viewSpaceType;
    glm::mat4 projectionMatrix;

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRJointSpace : public XRSpaceBase<XRJointSpace>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, xr::TrXRInputSource *inputSource, xr::TrXRJointIndex index);
    XRJointSpace(const Napi::CallbackInfo &info);

  public:
    void onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest);

  private:
    xr::TrXRJointIndex index;
    xr::TrXRInputSource *inputSource;

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRTargetRayOrGripSpace : public XRSpaceBase<XRTargetRayOrGripSpace>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, xr::TrXRInputSource *inputSource, bool isGrip);
    XRTargetRayOrGripSpace(const Napi::CallbackInfo &info);

  public:
    void onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest);

  private:
    xr::TrXRInputSource *inputSource;

  private:
    static Napi::FunctionReference *constructor;
  };
}
