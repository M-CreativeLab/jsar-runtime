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
#define MODULE_NAME "XRFrame"
    Napi::Function tpl = DefineClass(env, MODULE_NAME,
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
    env.Global().Set(MODULE_NAME, tpl);
#undef MODULE_NAME
  }

  XRFrame::XRFrame(const Napi::CallbackInfo &info)
      : XRHandleWrap<XRFrame, client_xr::XRFrame>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2 || !info[1].IsObject())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    Napi::Object sessionObj = info[1].As<Napi::Object>();
    session_ = XRSession::Unwrap(sessionObj);

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

    if (handle_->active() == false)
    {
      Napi::TypeError::New(env, NON_ACTIVE_MSG).ThrowAsJavaScriptException();
      return env.Undefined();
    }
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

    if (handle_->active() == false)
    {
      Napi::TypeError::New(env, NON_ACTIVE_MSG).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (
        info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject())
    {
      Napi::TypeError::New(env, "getPose() requires `XRSpace` and `XRSpace` parameters")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto space = XRSpace::Unwrap(info[0].As<Napi::Object>());
    auto baseSpace = XRSpace::Unwrap(info[1].As<Napi::Object>());
    try
    {
      auto pose = handle_->getPose(space->handle(), baseSpace->handle());
      return XRPose::NewInstance(env, pose);
    }
    catch (const std::invalid_argument &e)
    {
      auto consoleWarn = env.Global()
                             .Get("console")
                             .As<Napi::Object>()
                             .Get("warn")
                             .As<Napi::Function>();
      auto msg = "Failed to get pose: " + std::string(e.what());
      consoleWarn.Call({Napi::String::New(env, msg), info[0], info[1]});
      Napi::TypeError::New(env, msg)
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
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
