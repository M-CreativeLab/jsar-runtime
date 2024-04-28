#include "space.hpp"
#include "session.hpp"

namespace bindings
{
  Napi::FunctionReference *XRSpace::constructor;
  Napi::FunctionReference *XRReferenceSpace::constructor;
  Napi::FunctionReference *XRViewSpace::constructor;

  template <typename T>
  XRSpaceBase<T>::XRSpaceBase(const Napi::CallbackInfo &info, XRSpaceSubType subType) : Napi::ObjectWrap<T>(info),
                                                                                        subType(subType),
                                                                                        baseMatrix(mat4(1.0f)),
                                                                                        lastFrameId(-1),
                                                                                        mInverseMatrixCache(mat4(1.0f)),
                                                                                        mIsInverseMatrixDirty(true)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO: input source
  }

  template <typename T>
  void XRSpaceBase<T>::onPoseUpdate(XRSession *session, xr::DeviceFrame *frame)
  {
    mIsInverseMatrixDirty = true;
  }

  template <typename T>
  void XRSpaceBase<T>::ensurePoseUpdated(uint32_t frameId, XRSession *session, xr::DeviceFrame *frame)
  {
    if (lastFrameId == frameId)
      return;
    lastFrameId = frameId;
    static_cast<T*>(this)->onPoseUpdate(session, frame);
  }

  template <typename T>
  mat4 &XRSpaceBase<T>::getInverseBaseMatrix()
  {
    if (mIsInverseMatrixDirty == true)
    {
      mInverseMatrixCache = glm::inverse(baseMatrix);
      mIsInverseMatrixDirty = false;
    }
    return mInverseMatrixCache;
  }

  Napi::Object XRSpace::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRSpace", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRSpace", func);
    return exports;
  }

  XRSpace::XRSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info, XRSpaceSubType::TARGET_RAY)
  {
  }

  Napi::Object XRReferenceSpace::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRReferenceSpace", {InstanceMethod("getOffsetReferenceSpace", &XRReferenceSpace::GetOffsetReferenceSpace)});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRReferenceSpace", func);
    return exports;
  }

  Napi::Object XRReferenceSpace::NewInstance(Napi::Env env, XRReferenceSpaceType type)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    XRReferenceSpace *space = Napi::ObjectWrap<XRReferenceSpace>::Unwrap(obj);
    space->referenceSpaceType = type;
    return scope.Escape(obj).ToObject();
  }

  XRReferenceSpace::XRReferenceSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info, XRSpaceSubType::TARGET_RAY),
                                                                       offsetMatrix(mat4(1.0f))
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
      return; // just ignore invalid arguments

    auto type = info[0].As<Napi::String>().Utf8Value();
    if (type == "viewer")
      referenceSpaceType = XRReferenceSpaceType::VIEWER;
    else if (type == "local")
      referenceSpaceType = XRReferenceSpaceType::LOCAL;
    else if (type == "local-floor")
      referenceSpaceType = XRReferenceSpaceType::LOCAL_FLOOR;
    else if (type == "bounded-floor")
      referenceSpaceType = XRReferenceSpaceType::BOUNDED_FLOOR;
    else if (type == "unbounded")
      referenceSpaceType = XRReferenceSpaceType::UNBOUNDED;
    else
      Napi::TypeError::New(env, "Invalid reference space type").ThrowAsJavaScriptException();
  }

  Napi::Value XRReferenceSpace::GetOffsetReferenceSpace(const Napi::CallbackInfo &info)
  {
    // TODO
    return Napi::Value();
  }

  void XRReferenceSpace::onPoseUpdate(XRSession *session, xr::DeviceFrame *frame)
  {
    if (referenceSpaceType == XRReferenceSpaceType::VIEWER)
    {
      auto transform = frame->getViewerTransform();
      baseMatrix = createMat4FromArray(transform);
      XRSpaceBase<XRReferenceSpace>::onPoseUpdate(session, frame);
      DEBUG("Unity", "updated viewer matrix %d %d %d", baseMatrix[3].x, baseMatrix[3].y, baseMatrix[3].z);
    }
    else if (referenceSpaceType == XRReferenceSpaceType::LOCAL)
    {
      auto transform = frame->getLocalTransform(session->id);
      baseMatrix = createMat4FromArray(transform);
      XRSpaceBase<XRReferenceSpace>::onPoseUpdate(session, frame);
    }
    // TODO: other reference space types to update?
  }

  template <typename Ts>
  mat4 XRReferenceSpace::getRelativeTransform(XRSpaceBase<Ts> *baseSpace)
  {
    return baseMatrix * baseSpace->getInverseBaseMatrix();
  }

  mat4 XRReferenceSpace::getRelativeTransform(XRSpaceBase<XRSpace> *baseSpace)
  {
    return getRelativeTransform<XRSpace>(baseSpace);
  }

  mat4 XRReferenceSpace::getRelativeTransform(XRSpaceBase<XRReferenceSpace> *baseSpace)
  {
    return getRelativeTransform<XRReferenceSpace>(baseSpace);
  }

  mat4 XRReferenceSpace::getRelativeTransform(XRSpaceBase<XRViewSpace> *baseSpace)
  {
    return getRelativeTransform<XRViewSpace>(baseSpace);
  }

  Napi::Object XRViewSpace::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRViewSpace", {
                                                              InstanceAccessor("eye", &XRViewSpace::EyeGetter, nullptr),
                                                          });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    return exports;
  }

  Napi::Object XRViewSpace::NewInstance(Napi::Env env, XRViewSpaceType type)
  {
    Napi::EscapableHandleScope scope(env);
    auto viewSpaceType = Napi::External<XRViewSpaceType>::New(env, &type);
    Napi::Object obj = constructor->New({viewSpaceType});
    return scope.Escape(obj).ToObject();
  }

  XRViewSpace::XRViewSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info, XRSpaceSubType::TARGET_RAY)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "XRViewSpace constructor requires a type string").ThrowAsJavaScriptException();
      return;
    }

    if (info[0].IsExternal())
    {
      auto type = info[0].As<Napi::External<XRViewSpaceType>>().Data();
      viewSpaceType = *type;
    }
    else if (info[0].IsString())
    {
      auto type = info[0].As<Napi::String>().Utf8Value();
      if (type == "left")
        viewSpaceType = XRViewSpaceType::LEFT;
      else if (type == "right")
        viewSpaceType = XRViewSpaceType::RIGHT;
      else
        viewSpaceType = XRViewSpaceType::NONE;
    }
    else
    {
      Napi::TypeError::New(env, "Invalid view space type").ThrowAsJavaScriptException();
    }
  }

  Napi::Value XRViewSpace::EyeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::string eye;
    switch (viewSpaceType)
    {
    case XRViewSpaceType::LEFT:
      eye = "left";
      break;
    case XRViewSpaceType::RIGHT:
      eye = "right";
      break;
    default:
      eye = "none";
      break;
    }
    return Napi::String::New(env, eye);
  }

  void XRViewSpace::onPoseUpdate(XRSession *session, xr::DeviceFrame *frame)
  {
    auto transform = frame->getViewerTransform();
    baseMatrix = createMat4FromArray(transform);
    XRSpaceBase<XRViewSpace>::onPoseUpdate(session, frame);
  }

  XREye XRViewSpace::getEye()
  {
    return viewSpaceType;
  }

  template class XRSpaceBase<XRSpace>;
  template class XRSpaceBase<XRReferenceSpace>;
  template class XRSpaceBase<XRViewSpace>;
}
