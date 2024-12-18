#include "pose.hpp"
#include "./session.hpp"
#include "./frame.hpp"

namespace bindings
{
  using namespace std;
  using namespace glm;
  using namespace Napi;

  thread_local Napi::FunctionReference *XRPose::constructor;
  thread_local Napi::FunctionReference *XRViewerPose::constructor;
  thread_local Napi::FunctionReference *XRJointPose::constructor;

  // static
  void XRPose::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "XRPose",
                                     {InstanceAccessor("transform", &XRPose::TransformGetter, nullptr),
                                      InstanceAccessor("emulatedPosition", &XRPose::EmulatedPositionGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRPose", tpl);
  }

  // static
  Napi::Object XRPose::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRPose> handle)
  {
    assert(handle != nullptr);
    return XRPoseBase<XRPose>::NewInstance(env, handle);
  }

  // static
  Napi::Object XRPose::NewInstance(Napi::Env env, XRSession *session, XRFrame *frame, mat4 &transformationMatrix)
  {
    auto handle = make_shared<client_xr::XRPose>(session->handle(), frame->handle(), transformationMatrix);
    return NewInstance(env, handle);
  }

  XRPose::XRPose(const Napi::CallbackInfo &info)
      : XRPoseBase(info)
  {
    assert(handle_ != nullptr);
  }

  // static
  void XRViewerPose::Init(Napi::Env env)
  {
#define MODULE_NAME "XRViewerPose"
    Napi::Function tpl = DefineClass(
        env, MODULE_NAME,
        {InstanceAccessor("transform", &XRViewerPose::TransformGetter, nullptr),
         InstanceAccessor("emulatedPosition", &XRViewerPose::EmulatedPositionGetter, nullptr),
         InstanceAccessor("views", &XRViewerPose::ViewsGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set(MODULE_NAME, tpl);
#undef MODULE_NAME
  }

  // static
  Napi::Object XRViewerPose::NewInstance(Napi::Env env, shared_ptr<client_xr::XRViewerPose> handle)
  {
    assert(handle != nullptr);
    return XRPoseBase<XRViewerPose, client_xr::XRViewerPose>::NewInstance(env, handle);
  }

  XRViewerPose::XRViewerPose(const CallbackInfo &info)
      : XRPoseBase<XRViewerPose, client_xr::XRViewerPose>(info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    assert(handle_ != nullptr);
    for (auto viewHandle : handle_->views())
    {
      auto view = XRView::NewInstance(env, viewHandle);
      views.push_back(Persistent(view));
    }
  }

  XRViewerPose::~XRViewerPose()
  {
    for (size_t i = 0; i < views.size(); i++)
      views[i].Reset();
  }

  Value XRViewerPose::ViewsGetter(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    auto viewsArray = Array::New(env, views.size());
    for (size_t i = 0; i < views.size(); i++)
      viewsArray[i] = views[i].Value();
    return viewsArray;
  }

  // static
  void XRJointPose::Init(Napi::Env env)
  {
#define MODULE_NAME "XRJointPose"
    Function tpl = DefineClass(env, MODULE_NAME,
                               {InstanceValue("radius", Number::New(env, 0.0))});
    constructor = new FunctionReference();
    *constructor = Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set(MODULE_NAME, tpl);
#undef MODULE_NAME
  }

  // static
  Object XRJointPose::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRJointPose> handle)
  {
    assert(handle != nullptr);
    return XRPoseBase<XRJointPose, client_xr::XRJointPose>::NewInstance(env, handle);
  }

  XRJointPose::XRJointPose(const CallbackInfo &info)
      : XRPoseBase(info)
  {
  }
}
