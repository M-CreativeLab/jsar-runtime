#pragma once

#include <napi.h>
#include <glm/glm.hpp>
#include "common.hpp"
#include "xr/device.hpp"

using namespace std;
using namespace glm;

namespace bindings
{
  template <typename T>
  class XRSpaceBase : public Napi::ObjectWrap<T>
  {
  public:
    XRSpaceBase(const Napi::CallbackInfo &info, XRSpaceSubType subType);

  public:
    void onPoseUpdate(XRSession *session, xr::DeviceFrame *frame);
    void ensurePoseUpdated(uint32_t frameId, XRSession *session, xr::DeviceFrame *frame);
    mat4 &getInverseBaseMatrix();

  public:
    XRSpaceSubType subType;
    int32_t lastFrameId;
    mat4 baseMatrix;

  private:
    mat4 mInverseMatrixCache;
    bool mIsInverseMatrixDirty = true;
  };

  class XRSpace : public XRSpaceBase<XRSpace>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
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
    template <typename Ts>
    mat4 getRelativeTransform(XRSpaceBase<Ts> *baseSpace);
    mat4 getRelativeTransform(XRSpaceBase<XRSpace> *);
    mat4 getRelativeTransform(XRSpaceBase<XRReferenceSpace> *);
    mat4 getRelativeTransform(XRSpaceBase<XRViewSpace> *);

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
}
