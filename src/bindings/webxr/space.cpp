#include "space.hpp"
#include "session.hpp"
#include "math/matrix.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRSpace::constructor;
  thread_local Napi::FunctionReference *XRReferenceSpace::constructor;
  thread_local Napi::FunctionReference *XRViewSpace::constructor;
  thread_local Napi::FunctionReference *XRJointSpace::constructor;
  thread_local Napi::FunctionReference *XRTargetRayOrGripSpace::constructor;

  template <typename T>
  XRSpaceBase<T>::XRSpaceBase(const Napi::CallbackInfo &info,
                              XRSpaceSubType subType,
                              bool isReferenceSpace) : Napi::ObjectWrap<T>(info),
                                                       subType(subType),
                                                       isReferenceSpace(isReferenceSpace),
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
  void XRSpaceBase<T>::onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest)
  {
    mIsInverseMatrixDirty = true;
  }

  template <typename T>
  void XRSpaceBase<T>::ensurePoseUpdated(uint32_t frameId, XRSession *session, xr::TrXRFrameRequest *frameRequest)
  {
    if (lastFrameId == frameId)
      return;
    lastFrameId = frameId;
    static_cast<T *>(this)->onPoseUpdate(session, frameRequest);
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

  Napi::Object XRSpace::NewInstance(Napi::Env env, XRSpaceSubType subType, mat4 baseMatrix)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    XRSpace *space = Napi::ObjectWrap<XRSpace>::Unwrap(obj);
    space->subType = subType;
    space->baseMatrix = baseMatrix;
    return scope.Escape(obj).ToObject();
  }

  XRSpace::XRSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info, XRSpaceSubType::UNSET)
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

  Napi::Object XRReferenceSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRReferenceSpace> handle)
  {
    Napi::EscapableHandleScope scope(env);
    SharedReference<client_xr::XRReferenceSpace> sharedHandle(handle);

    auto handleExternal = Napi::External<SharedReference<client_xr::XRReferenceSpace>>::New(env, &sharedHandle);
    Napi::Object instance = constructor->New({handleExternal});
    return scope.Escape(instance).ToObject();
  }

  XRReferenceSpace::XRReferenceSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info, XRSpaceSubType::UNSET, true),
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

  void XRReferenceSpace::onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest)
  {
    if (referenceSpaceType == XRReferenceSpaceType::VIEWER)
    {
      baseMatrix = createMat4FromArray(frameRequest->viewerBaseMatrix);
      XRSpaceBase<XRReferenceSpace>::onPoseUpdate(session, frameRequest);
    }
    else if (referenceSpaceType == XRReferenceSpaceType::LOCAL)
    {
      baseMatrix = createMat4FromArray(frameRequest->localBaseMatrix);
      XRSpaceBase<XRReferenceSpace>::onPoseUpdate(session, frameRequest);
    }
    else if (referenceSpaceType == XRReferenceSpaceType::UNBOUNDED)
    {
      auto localOffset = createMat4FromArray(frameRequest->localBaseMatrix);
      baseMatrix = localOffset * math::getOriginMatrix();
      XRSpaceBase<XRReferenceSpace>::onPoseUpdate(session, frameRequest);
    }
    // TODO: other reference space types to update?
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

    projectionMatrix = mat4(1.0f);
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

  void XRViewSpace::onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest)
  {
    if (viewSpaceType == XRViewSpaceType::NONE)
      return; // No need to update pose if the type is none.
    if (viewSpaceType != frameRequest->viewIndex)
    {
      std::cerr << "Failed to update pose for XRViewSpace()" << std::endl;
      return;
    }
    // TODO: check if a device
    auto &view = frameRequest->views[frameRequest->viewIndex];
    baseMatrix = glm::inverse(view.getViewMatrix());
    projectionMatrix = view.getProjectionMatrix();
    XRSpaceBase<XRViewSpace>::onPoseUpdate(session, frameRequest);
  }

  XREye XRViewSpace::getEye()
  {
    return viewSpaceType;
  }

  glm::mat4 &XRViewSpace::getProjectionMatrix()
  {
    return projectionMatrix;
  }

  Napi::Object XRJointSpace::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRJointSpace", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    return exports;
  }

  Napi::Object XRJointSpace::NewInstance(Napi::Env env, xr::TrXRInputSource *inputSource, xr::TrXRJointIndex index)
  {
    Napi::EscapableHandleScope scope(env);
    auto inputSourceExternal = Napi::External<xr::TrXRInputSource>::New(env, inputSource);
    Napi::String jointName;
    switch (index)
    {
    case xr::TrXRJointIndex::JointWrist:
      jointName = Napi::String::New(env, "wrist");
      break;
    case xr::TrXRJointIndex::JointThumbMetacarpal:
      jointName = Napi::String::New(env, "thumb-metacarpal");
      break;
    case xr::TrXRJointIndex::JointThumbPhalanxProximal:
      jointName = Napi::String::New(env, "thumb-phalanx-proximal");
      break;
    case xr::TrXRJointIndex::JointThumbPhalanxDistal:
      jointName = Napi::String::New(env, "thumb-phalanx-distal");
      break;
    case xr::TrXRJointIndex::JointThumbTip:
      jointName = Napi::String::New(env, "thumb-tip");
      break;
    case xr::TrXRJointIndex::JointIndexFingerMetacarpal:
      jointName = Napi::String::New(env, "index-finger-metacarpal");
      break;
    case xr::TrXRJointIndex::JointIndexFingerPhalanxProximal:
      jointName = Napi::String::New(env, "index-finger-phalanx-proximal");
      break;
    case xr::TrXRJointIndex::JointIndexFingerPhalanxIntermediate:
      jointName = Napi::String::New(env, "index-finger-phalanx-intermediate");
      break;
    case xr::TrXRJointIndex::JointIndexFingerPhalanxDistal:
      jointName = Napi::String::New(env, "index-finger-phalanx-distal");
      break;
    case xr::TrXRJointIndex::JointIndexFingerTip:
      jointName = Napi::String::New(env, "index-finger-tip");
      break;
    case xr::TrXRJointIndex::JointMiddleFingerMetacarpal:
      jointName = Napi::String::New(env, "middle-finger-metacarpal");
      break;
    case xr::TrXRJointIndex::JointMiddleFingerPhalanxProximal:
      jointName = Napi::String::New(env, "middle-finger-phalanx-proximal");
      break;
    case xr::TrXRJointIndex::JointMiddleFingerPhalanxIntermediate:
      jointName = Napi::String::New(env, "middle-finger-phalanx-intermediate");
      break;
    case xr::TrXRJointIndex::JointMiddleFingerPhalanxDistal:
      jointName = Napi::String::New(env, "middle-finger-phalanx-distal");
      break;
    case xr::TrXRJointIndex::JointMiddleFingerTip:
      jointName = Napi::String::New(env, "middle-finger-tip");
      break;
    case xr::TrXRJointIndex::JointRingFingerMetacarpal:
      jointName = Napi::String::New(env, "ring-finger-metacarpal");
      break;
    case xr::TrXRJointIndex::JointRingFingerPhalanxProximal:
      jointName = Napi::String::New(env, "ring-finger-phalanx-proximal");
      break;
    case xr::TrXRJointIndex::JointRingFingerPhalanxIntermediate:
      jointName = Napi::String::New(env, "ring-finger-phalanx-intermediate");
      break;
    case xr::TrXRJointIndex::JointRingFingerPhalanxDistal:
      jointName = Napi::String::New(env, "ring-finger-phalanx-distal");
      break;
    case xr::TrXRJointIndex::JointRingFingerTip:
      jointName = Napi::String::New(env, "ring-finger-tip");
      break;
    case xr::TrXRJointIndex::JointPinkyFingerMetacarpal:
      jointName = Napi::String::New(env, "pinky-finger-metacarpal");
      break;
    case xr::TrXRJointIndex::JointPinkyFingerPhalanxProximal:
      jointName = Napi::String::New(env, "pinky-finger-phalanx-proximal");
      break;
    case xr::TrXRJointIndex::JointPinkyFingerPhalanxIntermediate:
      jointName = Napi::String::New(env, "pinky-finger-phalanx-intermediate");
      break;
    case xr::TrXRJointIndex::JointPinkyFingerPhalanxDistal:
      jointName = Napi::String::New(env, "pinky-finger-phalanx-distal");
      break;
    case xr::TrXRJointIndex::JointPinkyFingerTip:
      jointName = Napi::String::New(env, "pinky-finger-tip");
      break;
    default:
      jointName = Napi::String::New(env, "unknown");
      break;
    }
    Napi::Object obj = constructor->New({inputSourceExternal, jointName, Napi::Number::New(env, static_cast<int>(index))});
    return scope.Escape(obj).ToObject();
  }

  XRJointSpace::XRJointSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info, XRSpaceSubType::UNSET)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "XRJointSpace constructor expects 3 arguments").ThrowAsJavaScriptException();
      return;
    }
    if (!info[0].IsExternal())
    {
      Napi::TypeError::New(env, "XRJointSpace constructor expects an external as the first argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "XRJointSpace constructor expects a string as the second argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "XRJointSpace constructor expects a number as the third argument").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<Napi::External<xr::TrXRInputSource>>();
    inputSource = external.Data();
    auto jointName = info[1].As<Napi::String>().Utf8Value();
    auto index = info[2].As<Napi::Number>().Int32Value();
    this->index = static_cast<xr::TrXRJointIndex>(index);
    info.This().ToObject().Set("jointName", Napi::String::New(env, jointName));
  }

  void XRJointSpace::onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest)
  {
    baseMatrix = glm::make_mat4(inputSource->joints[static_cast<int>(index)].baseMatrix);
    XRSpaceBase<XRJointSpace>::onPoseUpdate(session, frameRequest);
  }

  Napi::Object XRTargetRayOrGripSpace::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRSpace" /** use the same name as XRSpace */, {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    return exports;
  }

  Napi::Object XRTargetRayOrGripSpace::NewInstance(Napi::Env env, xr::TrXRInputSource *inputSource, bool isGrip)
  {
    Napi::EscapableHandleScope scope(env);
    auto instance = Napi::External<xr::TrXRInputSource>::New(env, inputSource);
    Napi::Object obj = constructor->New({Napi::Boolean::New(env, isGrip), instance});
    return scope.Escape(obj).ToObject();
  }

  XRTargetRayOrGripSpace::XRTargetRayOrGripSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info, XRSpaceSubType::TARGET_RAY)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "XRTargetRayOrGripSpace constructor expects 2 arguments").ThrowAsJavaScriptException();
      return;
    }
    if (!info[0].IsBoolean())
    {
      Napi::TypeError::New(env, "XRTargetRayOrGripSpace constructor expects a boolean as the first argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[1].IsExternal())
    {
      Napi::TypeError::New(env, "XRTargetRayOrGripSpace constructor could not be called").ThrowAsJavaScriptException();
      return;
    }

    auto isGrip = info[0].As<Napi::Boolean>().Value();
    auto external = info[1].As<Napi::External<xr::TrXRInputSource>>();
    inputSource = external.Data();
    subType = isGrip ? XRSpaceSubType::GRIP : XRSpaceSubType::TARGET_RAY;
  }

  void XRTargetRayOrGripSpace::onPoseUpdate(XRSession *session, xr::TrXRFrameRequest *frameRequest)
  {
    if (subType == XRSpaceSubType::GRIP)
      baseMatrix = glm::make_mat4(inputSource->gripBaseMatrix);
    else if (subType == XRSpaceSubType::TARGET_RAY)
      baseMatrix = glm::make_mat4(inputSource->targetRayBaseMatrix);
    XRSpaceBase<XRTargetRayOrGripSpace>::onPoseUpdate(session, frameRequest);
  }

  template class XRSpaceBase<XRSpace>;
  template class XRSpaceBase<XRReferenceSpace>;
  template class XRSpaceBase<XRViewSpace>;
  template class XRSpaceBase<XRJointSpace>;
  template class XRSpaceBase<XRTargetRayOrGripSpace>;
}
