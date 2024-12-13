#include "frame.hpp"
#include "space.hpp"
#include "pose.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRFrame::constructor;
  thread_local uint32_t XRFrame::NEXT_FRAME_ID = 0;

  // static
  void XRFrame::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "XRFrame",
                                     {
                                         InstanceMethod("createAnchor", &XRFrame::CreateAnchor),
                                         InstanceMethod("getHitTestResults", &XRFrame::GetHitTestResults),
                                         InstanceMethod("getHitTestResultsForTransientInput", &XRFrame::GetHitTestResultsForTransientInput),
                                         InstanceMethod("fillPoses", &XRFrame::FillPoses),
                                         InstanceMethod("getJointPose", &XRFrame::GetJointPose),
                                         InstanceMethod("fillJointRadii", &XRFrame::FillJointRadii),
                                         InstanceMethod("getImageTrackingResults", &XRFrame::GetImageTrackingResults),
                                         InstanceMethod("getLightEstimate", &XRFrame::GetLightEstimate),
                                         InstanceMethod("getDepthInformation", &XRFrame::GetDepthInformation),
                                         InstanceMethod("getPose", &XRFrame::GetPose),
                                         InstanceMethod("getViewerPose", &XRFrame::GetViewerPose),
                                     });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.Global().Set("XRFrame", tpl);
  }

  Napi::Object XRFrame::NewInstance(Napi::Env env, XRSession *session, std::shared_ptr<client_xr::XRFrame> frame)
  {
    Napi::EscapableHandleScope scope(env);
    auto handleRef = SharedReference<client_xr::XRFrame>(frame);
    auto handleExternal = Napi::External<SharedReference<client_xr::XRFrame>>::New(env, &handleRef);
    Napi::Object instance = XRFrame::constructor->New({session->Value(), handleExternal});
    return scope.Escape(instance).ToObject();
  }

  XRFrame::XRFrame(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<XRFrame>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "Invalid arguments number to create XRFrame(session, [[native frame]]).").ThrowAsJavaScriptException();
      return;
    }
    if (!info[0].IsObject() || !info[0].ToObject().InstanceOf(XRSession::constructor->Value()))
    {
      Napi::TypeError::New(env, "XRFrame constructor requires a `XRSession` object.").ThrowAsJavaScriptException();
      return;
    }
    if (!info[1].IsExternal())
    {
      Napi::TypeError::New(env, "Create a XRFrame object from JavaScript is not allowed.").ThrowAsJavaScriptException();
      return;
    }

    Napi::Object sessionObj = info[0].As<Napi::Object>();
    session_ = XRSession::Unwrap(sessionObj);

    auto handleExternal = info[1].As<Napi::External<SharedReference<client_xr::XRFrame>>>();
    auto handleRef = handleExternal.Data();
    if (handleRef == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
    handle_ = handleRef->value;

    auto jsThis = info.This().ToObject();
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("session", sessionObj, napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("trackedAnchor", env.Null(), napi_enumerable)); // TODO: support trackedAnchor

    // Properties added by JSAR
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("_id",
                                                          Napi::Number::New(env, id()),
                                                          napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("_stereoId",
                                                          Napi::Number::New(env, handle_->stereoId()),
                                                          napi_enumerable));
  }

#define NOT_IMPLEMENTED_YET(method)                                                                    \
  Napi::Value XRFrame::method(const Napi::CallbackInfo &info)                                          \
  {                                                                                                    \
    Napi::Env env = info.Env();                                                                        \
    Napi::HandleScope scope(env);                                                                      \
    Napi::TypeError::New(env, "Method(" #method ") not implemented yet").ThrowAsJavaScriptException(); \
    return env.Undefined();                                                                            \
  }

  NOT_IMPLEMENTED_YET(CreateAnchor)
  NOT_IMPLEMENTED_YET(GetHitTestResults)
  NOT_IMPLEMENTED_YET(GetHitTestResultsForTransientInput)
  NOT_IMPLEMENTED_YET(FillPoses)
  NOT_IMPLEMENTED_YET(FillJointRadii)
  NOT_IMPLEMENTED_YET(GetImageTrackingResults)
  NOT_IMPLEMENTED_YET(GetLightEstimate)
  NOT_IMPLEMENTED_YET(GetDepthInformation)
#undef NOT_IMPLEMENTED_YET

#define NON_ANIMFRAME_MSG "getViewerPose can only be called on XRFrame objects passed to XRSession.requestAnimationFrame callbacks."
#define NON_ACTIVE_MSG "XRFrame access outside the callback that produced it is invalid."

  Napi::Value XRFrame::GetViewerPose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "getViewerPose requires a reference space object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto referenceSpace = XRReferenceSpace::Unwrap(info[0].ToObject());
    auto viewerSpace = handle_->getViewerPose(referenceSpace->handle());
    return XRViewerPose::NewInstance(env, viewerSpace);
  }

  Napi::Value XRFrame::GetJointPose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (active == false)
    // {
    //   Napi::TypeError::New(env, NON_ACTIVE_MSG).ThrowAsJavaScriptException();
    //   return env.Undefined();
    // }

    if (
        info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject())
    {
      Napi::TypeError::New(env, "getJointPose() requires `XRJointSpace` and `XRSpace` parameters")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jointSpace = XRJointSpace::Unwrap(info[0].As<Napi::Object>());
    auto baseSpace = XRSpace::Unwrap(info[1].As<Napi::Object>());
    auto pose = handle_->getJointPose(jointSpace->handle(), baseSpace->handle());
    return XRJointPose::NewInstance(env, pose);
  }

  Napi::Value XRFrame::GetPose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (active == false)
    // {
    //   Napi::TypeError::New(env, NON_ACTIVE_MSG).ThrowAsJavaScriptException();
    //   return env.Undefined();
    // }

    // if (info.Length() < 2)
    // {
    //   Napi::TypeError::New(env, "getPose requires a reference space object and an XRSpace object").ThrowAsJavaScriptException();
    //   return env.Undefined();
    // }

    if (
        info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject())
    {
      Napi::TypeError::New(env, "getPose() requires `XRSpace` and `XRSpace` parameters")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto space = XRSpace::Unwrap(info[0].As<Napi::Object>());
    auto baseSpace = XRSpace::Unwrap(info[1].As<Napi::Object>());
    auto pose = handle_->getPose(space->handle(), baseSpace->handle());
    return XRPose::NewInstance(env, pose);
  }

  void XRFrame::start()
  {
    handle_->startFrame();
  }

  void XRFrame::end()
  {
    handle_->endFrame();
  }
}
