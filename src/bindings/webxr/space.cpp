#include "space-inl.hpp"
#include "session.hpp"
#include "math/matrix.hpp"

namespace bindings
{
  using namespace std;
  using namespace Napi;

  thread_local FunctionReference *XRSpace::constructor;
  thread_local FunctionReference *XRReferenceSpace::constructor;
  thread_local FunctionReference *XRViewSpace::constructor;
  thread_local FunctionReference *XRJointSpace::constructor;
  thread_local FunctionReference *XRTargetRayOrGripSpace::constructor;

  // static
  void XRSpace::Init(Napi::Env env)
  {
    HandleScope scope(env);
    Function func = DefineClass(env, "XRSpace", {});
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    env.Global().Set("XRSpace", func);
  }

  // static
  Object XRSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRSpace> handle)
  {
    return XRSpaceBase<XRSpace>::NewInstance(env, handle);
  }

  // static
  Object XRSpace::NewInstance(Napi::Env env, client_xr::XRSpaceSubType subType, mat4 baseMatrix)
  {
    return NewInstance(env, make_shared<client_xr::XRSpace>(baseMatrix, subType));
  }

  XRSpace::XRSpace(const CallbackInfo &info) : XRSpaceBase(info)
  {
  }

  // static
  void XRReferenceSpace::Init(Napi::Env env)
  {
#define MODULE_NAME "XRReferenceSpace"
    HandleScope scope(env);
    Function func = DefineClass(env, MODULE_NAME,
                                {InstanceMethod("getOffsetReferenceSpace", &XRReferenceSpace::GetOffsetReferenceSpace)});
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  // static
  Object XRReferenceSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRReferenceSpace> handle)
  {
    return XRSpaceBase<XRReferenceSpace, client_xr::XRReferenceSpace>::NewInstance(env, handle);
  }

  // static
  Object XRReferenceSpace::NewInstance(Napi::Env env, client_xr::XRReferenceSpaceType type)
  {
    return NewInstance(env, make_shared<client_xr::XRReferenceSpace>(type));
  }

  XRReferenceSpace::XRReferenceSpace(const CallbackInfo &info) : XRSpaceBase(info)
  {
  }

  Value XRReferenceSpace::GetOffsetReferenceSpace(const CallbackInfo &info)
  {
    // TODO
    return Value();
  }

  // static
  void XRViewSpace::Init(Napi::Env env)
  {
    HandleScope scope(env);
    Function func = DefineClass(env, "XRViewSpace",
                                {
                                    InstanceAccessor("eye", &XRViewSpace::EyeGetter, nullptr),
                                });
    constructor = new FunctionReference();
    *constructor = Persistent(func);
  }

  // static
  Object XRViewSpace::NewInstance(Napi::Env env, client_xr::XRViewSpaceType type)
  {
    auto handle = client_xr::XRViewSpace::Make(type);
    return XRSpaceBase<XRViewSpace, client_xr::XRViewSpace>::NewInstance(env, handle);
  }

  XRViewSpace::XRViewSpace(const CallbackInfo &info)
      : XRSpaceBase(info)
  {
  }

  Value XRViewSpace::EyeGetter(const CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    HandleScope scope(env);

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
    return String::New(env, eye);
  }

  // static
  void XRJointSpace::Init(Napi::Env env)
  {
#define MODULE_NAME "XRJointSpace"
    HandleScope scope(env);
    Function func = DefineClass(env, MODULE_NAME, {});
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  // static
  Object XRJointSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRJointSpace> handle)
  {
    return XRSpaceBase<XRJointSpace, client_xr::XRJointSpace>::NewInstance(env, handle);
  }

  // static
  Object XRJointSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRInputSource> inputSource, client_xr::XRJointIndex index)
  {
    return NewInstance(env, client_xr::XRJointSpace::Make(inputSource, index));
  }

  XRJointSpace::XRJointSpace(const CallbackInfo &info) : XRSpaceBase(info)
  {
    Napi::Env env = info.Env();
    HandleScope scope(env);

    auto jsThis = info.This().ToObject();
    jsThis.Set("jointName", Napi::String::New(env, handle_->name));
  }

  // static
  void XRTargetRayOrGripSpace::Init(Napi::Env env)
  {
    HandleScope scope(env);
    Function func = DefineClass(env, "XRSpace" /** use the same name as XRSpace */, {});
    constructor = new FunctionReference();
    *constructor = Persistent(func);
  }

  // static
  Object XRTargetRayOrGripSpace::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRTargetRayOrGripSpace> handle)
  {
    return XRSpaceBase<XRTargetRayOrGripSpace, client_xr::XRTargetRayOrGripSpace>::NewInstance(env, handle);
  }

  // static
  Object XRTargetRayOrGripSpace::NewInstance(Napi::Env env, shared_ptr<client_xr::XRInputSource> inputSource, bool isGrip)
  {
    auto handle = client_xr::XRTargetRayOrGripSpace::Make(
        inputSource,
        isGrip ? client_xr::XRSpaceSubType::kGrip : client_xr::XRSpaceSubType::kTargetRay);
    return NewInstance(env, handle);
  }

  XRTargetRayOrGripSpace::XRTargetRayOrGripSpace(const CallbackInfo &info) : XRSpaceBase(info)
  {
  }
}
