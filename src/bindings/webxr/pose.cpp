#include "pose.hpp"
#include "./session.hpp"
#include "./frame.hpp"

namespace bindings
{
  using namespace std;
  using namespace glm;

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
  }

  // static
  void XRViewerPose::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "XRViewerPose",
                                     {InstanceAccessor("transform", &XRViewerPose::TransformGetter, nullptr),
                                      InstanceAccessor("emulatedPosition", &XRViewerPose::EmulatedPositionGetter, nullptr),
                                      InstanceAccessor("views", &XRViewerPose::ViewsGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRViewerPose", tpl);
  }

  // static
  Napi::Object XRViewerPose::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRViewerPose> handle)
  {
    return XRPoseBase<XRViewerPose, client_xr::XRViewerPose>::NewInstance(env, handle);
  }

  // static
  Napi::Object XRViewerPose::NewInstance(Napi::Env env, XRSession *session, mat4 &transformationMatrix,
                                         XRFrame &frame,
                                         XRReferenceSpace &baseSpace)
  {
    auto handle = make_shared<client_xr::XRViewerPose>(session->handle(), frame.handle(), transformationMatrix, baseSpace.handle());
    return NewInstance(env, handle);
  }

  XRViewerPose::XRViewerPose(const Napi::CallbackInfo &info)
      : XRPoseBase(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    for (auto viewHandle : handle_->views())
    {
      auto view = XRView::NewInstance(env, viewHandle);
      views.push_back(Napi::Persistent(view));
    }
  }

  XRViewerPose::~XRViewerPose()
  {
    for (size_t i = 0; i < views.size(); i++)
      views[i].Reset();
  }

  Napi::Value XRViewerPose::ViewsGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Array viewsArray = Napi::Array::New(env, views.size());
    for (size_t i = 0; i < views.size(); i++)
      viewsArray[i] = views[i].Value();
    return viewsArray;
  }

  // static
  void XRJointPose::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "XRJointPose",
                                     {InstanceValue("radius", Napi::Number::New(env, 0.0))});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRJointPose", tpl);
  }

  // static
  Napi::Object XRJointPose::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRJointPose> handle)
  {
    return XRPoseBase<XRJointPose, client_xr::XRJointPose>::NewInstance(env, handle);
  }

  XRJointPose::XRJointPose(const Napi::CallbackInfo &info)
      : XRPoseBase(info)
  {
  }
}
