#include "frame.hpp"
#include "space.hpp"
#include "pose.hpp"

namespace bindings
{
  Napi::FunctionReference *XRFrame::constructor;
  uint32_t XRFrame::NEXT_FRAME_ID = 0;

  Napi::Object XRFrame::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRFrame",
                                     {
                                         InstanceAccessor("session", &XRFrame::SessionGetter, nullptr),
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
    env.SetInstanceData(constructor);
    exports.Set("XRFrame", tpl);
    return exports;
  }

  Napi::Object XRFrame::NewInstance(Napi::Env env, xr::DeviceFrame *frame, XRSession *session)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({session->Value()});
    XRFrame *instance = XRFrame::Unwrap(obj);
    instance->internal = frame;
    return scope.Escape(obj).ToObject();
  }

  XRFrame::XRFrame(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRFrame>(info),
                                                     id(NEXT_FRAME_ID++),
                                                     active(false),
                                                     animationFrame(false)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "XRFrame constructor requires a session object").ThrowAsJavaScriptException();
      return;
    }

    Napi::Object sessionObj = info[0].As<Napi::Object>();
    session = XRSession::Unwrap(sessionObj);
    sessionId = session->id;
    device = session->device;
  }

  Napi::Value XRFrame::SessionGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
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
  NOT_IMPLEMENTED_YET(GetJointPose)
#undef NOT_IMPLEMENTED_YET

#define NON_ANIMFRAME_MSG "getViewerPose can only be called on XRFrame objects passed to XRSession.requestAnimationFrame callbacks."
#define NON_ACTIVE_MSG "XRFrame access outside the callback that produced it is invalid."

  Napi::Value XRFrame::GetViewerPose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (animationFrame == false)
    {
      Napi::TypeError::New(env, NON_ANIMFRAME_MSG).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (active == false)
    {
      Napi::TypeError::New(env, NON_ACTIVE_MSG).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getViewerPose requires a reference space object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto referenceSpace = XRReferenceSpace::Unwrap(info[0].As<Napi::Object>());
    auto viewerSpace = session->getViewerSpace();
    referenceSpace->ensurePoseUpdated(id, session, internal);
    viewerSpace->ensurePoseUpdated(id, session, internal);

    auto activeEye = getActiveEye();
    auto viewerTransform = referenceSpace->getRelativeTransform(viewerSpace);
    auto viewerPoseObject = XRViewerPose::NewInstance(env, viewerTransform);
    auto viewerPoseUnwrapped = XRViewerPose::Unwrap(viewerPoseObject);

    session->iterateViewSpaces([this, env, viewerPoseUnwrapped, activeEye, referenceSpace](XRViewSpace *viewSpace, uint32_t viewIndex, XRSession *session)
                               {
                                 if (activeEye != XREye::NONE && viewSpace->getEye() != activeEye)
                                   return;
                                 viewSpace->ensurePoseUpdated(id, session, internal);

                                 auto viewTransform = referenceSpace->getRelativeTransform(viewSpace);
                                 auto xrView = XRView::NewInstance(env, session, viewTransform, viewIndex, activeEye);
                                 viewerPoseUnwrapped->addView(xrView);
                                 // End
                               });
    return viewerPoseObject;
  }

  Napi::Value XRFrame::GetPose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (active == false)
    {
      Napi::TypeError::New(env, NON_ACTIVE_MSG).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getPose requires a reference space object and an XRSpace object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto space = XRSpace::Unwrap(info[0].As<Napi::Object>());
    auto baseSpace = XRSpace::Unwrap(info[1].As<Napi::Object>());

    // TODO
    return env.Undefined();
  }

  uint32_t XRFrame::getStereoRenderingId()
  {
    return internal->getCurrentStereoId();
  }

  uint32_t XRFrame::getViewIndex()
  {
    if (getActiveEye() == XREye::RIGHT)
      return 1;
    else
      return 0;
  }

  XREye XRFrame::getActiveEye()
  {
    auto device = xr::Device::GetInstance();
    if (device->getStereoRenderingMode() == xr::StereoRenderingMode::MultiPass)
    {
      auto multipassFrame = static_cast<xr::MultiPassFrame *>(internal);
      auto activeEye = multipassFrame->getActiveEyeId();
      return activeEye == 0 ? XREye::LEFT : XREye::RIGHT;
    }
    else
    {
      return XREye::NONE;
    }
  }

  void XRFrame::start()
  {
    active = true;
    animationFrame = true;
    device->startFrame(sessionId, getStereoRenderingId(), getViewIndex());
  }

  void XRFrame::end()
  {
    active = false;
    // animationFrame = false;
    device->endFrame(sessionId, getStereoRenderingId(), getViewIndex());
  }
}
