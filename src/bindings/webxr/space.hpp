#pragma once

#include <napi.h>
#include <glm/glm.hpp>
#include "common.hpp"
#include "xr/device.hpp"

using namespace std;
using namespace glm;

namespace bindings
{
  #define XRSPACE_RELATIVE_TRANSFORM(space, baseSpace) space->baseMatrix * baseSpace->getInverseBaseMatrix()

  template <typename T>
  class XRSpaceBase : public Napi::ObjectWrap<T>
  {
  public:
    XRSpaceBase(const Napi::CallbackInfo &info, XRSpaceSubType subType, bool isReferenceSpace = false);

  public:
    void onPoseUpdate(XRSession *session, xr::DeviceFrame *frame);
    void ensurePoseUpdated(uint32_t frameId, XRSession *session, xr::DeviceFrame *frame);
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
    void onPoseUpdate(XRSession *session, xr::DeviceFrame *frame);

  private:
    XRReferenceSpaceType referenceSpaceType;
    mat4 offsetMatrix;

  private:
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
    void onPoseUpdate(XRSession *session, xr::DeviceFrame *frame);
    XREye getEye();

  private:
    XRViewSpaceType viewSpaceType;

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRTargetRayOrGripSpace : public XRSpaceBase<XRTargetRayOrGripSpace>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, xr::InputSource* inputSource, bool isGrip);
    XRTargetRayOrGripSpace(const Napi::CallbackInfo &info);

  public:
    void onPoseUpdate(XRSession *session, xr::DeviceFrame *frame);

  private:
    xr::InputSource* inputSource;

  private:
    static Napi::FunctionReference *constructor;
  };
}
