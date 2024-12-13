#include "space-inl.hpp"
#include "session.hpp"
#include "math/matrix.hpp"

namespace bindings
{
  using namespace std;

  thread_local Napi::FunctionReference *XRSpace::constructor;
  thread_local Napi::FunctionReference *XRReferenceSpace::constructor;
  thread_local Napi::FunctionReference *XRViewSpace::constructor;
  thread_local Napi::FunctionReference *XRJointSpace::constructor;
  thread_local Napi::FunctionReference *XRTargetRayOrGripSpace::constructor;

  // static
  void XRSpace::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRSpace", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("XRSpace", func);
  }

  // static
  Napi::Object XRSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRSpace> handle)
  {
    return XRSpaceBase<XRSpace>::NewInstance(env, handle);
  }

  // static
  Napi::Object XRSpace::NewInstance(Napi::Env env, client_xr::XRSpaceSubType subType, mat4 baseMatrix)
  {
    return NewInstance(env, make_shared<client_xr::XRSpace>(baseMatrix, subType));
  }

  XRSpace::XRSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info)
  {
  }

  // static
  void XRReferenceSpace::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRReferenceSpace",
                                      {InstanceMethod("getOffsetReferenceSpace", &XRReferenceSpace::GetOffsetReferenceSpace)});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("XRReferenceSpace", func);
  }

  // static
  Napi::Object XRReferenceSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRReferenceSpace> handle)
  {
    return XRSpaceBase<XRReferenceSpace, client_xr::XRReferenceSpace>::NewInstance(env, handle);
  }

  // static
  Napi::Object XRReferenceSpace::NewInstance(Napi::Env env, client_xr::XRReferenceSpaceType type)
  {
    return NewInstance(env, make_shared<client_xr::XRReferenceSpace>(type));
  }

  XRReferenceSpace::XRReferenceSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info)
  {
  }

  Napi::Value XRReferenceSpace::GetOffsetReferenceSpace(const Napi::CallbackInfo &info)
  {
    // TODO
    return Napi::Value();
  }

  // static
  void XRViewSpace::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRViewSpace",
                                      {
                                          InstanceAccessor("eye", &XRViewSpace::EyeGetter, nullptr),
                                      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
  }

  // static
  Napi::Object XRViewSpace::NewInstance(Napi::Env env, client_xr::XRViewSpaceType type)
  {
    auto handle = client_xr::XRViewSpace::Make(type);
    return XRSpaceBase<XRViewSpace, client_xr::XRViewSpace>::NewInstance(env, handle);
  }

  XRViewSpace::XRViewSpace(const Napi::CallbackInfo &info)
      : XRSpaceBase(info)
  {
  }

  Napi::Value XRViewSpace::EyeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    string eye;
    switch (handle_->eye())
    {
    case client_xr::XREye::kLeft:
      eye = "left";
      break;
    case client_xr::XREye::kRight:
      eye = "right";
      break;
    default:
      eye = "none";
      break;
    }
    return Napi::String::New(env, eye);
  }

  // static
  void XRJointSpace::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRJointSpace", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("XRJointSpace", func);
  }

  // static
  Napi::Object XRJointSpace::NewInstance(Napi::Env env, xr::TrXRInputSource *inputSource, client_xr::XRJointIndex index)
  {
    auto handle = make_shared<client_xr::XRJointSpace>(nullptr, index);
    return XRSpaceBase<XRJointSpace, client_xr::XRJointSpace>::NewInstance(env, handle);
  }

  XRJointSpace::XRJointSpace(const Napi::CallbackInfo &info)
      : XRSpaceBase(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto jsThis = info.This().ToObject();
    jsThis.Set("jointName", Napi::String::New(env, handle_->name));
  }

  // static
  void XRTargetRayOrGripSpace::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "XRSpace" /** use the same name as XRSpace */, {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
  }

  // static
  Napi::Object XRTargetRayOrGripSpace::NewInstance(Napi::Env env, xr::TrXRInputSource *inputSource, bool isGrip)
  {
    auto handle = make_shared<client_xr::XRTargetRayOrGripSpace>(nullptr,
                                                                 isGrip ? client_xr::XRSpaceSubType::kGrip : client_xr::XRSpaceSubType::kTargetRay);
    return XRSpaceBase<XRTargetRayOrGripSpace, client_xr::XRTargetRayOrGripSpace>::NewInstance(env, handle);
  }

  XRTargetRayOrGripSpace::XRTargetRayOrGripSpace(const Napi::CallbackInfo &info) : XRSpaceBase(info)
  {
  }
}
