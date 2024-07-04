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

  Napi::Object XRFrame::NewInstance(Napi::Env env, xr::TrXRFrameRequest *frameRequest, XRSession *session)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = XRFrame::constructor->New({session->Value(), Napi::External<xr::TrXRFrameRequest>::New(env, frameRequest)});
    return scope.Escape(napi_value(obj)).ToObject();
  }

  XRFrame::XRFrame(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRFrame>(info),
                                                     active(false),
                                                     animationFrame(false)
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
    session = XRSession::Unwrap(sessionObj);
    sessionId = session->id;
    device = session->device;

    Napi::External<xr::TrXRFrameRequest> external = info[1].As<Napi::External<xr::TrXRFrameRequest>>();
    internal = external.Data();
    id = internal->id;

    auto jsThis = info.This().ToObject();
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("session", sessionObj, napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("trackedAnchor", env.Null(), napi_enumerable)); // TODO: support trackedAnchor
    // Properties added by JSAR
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("_id", Napi::Number::New(env, id), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("_stereoId", Napi::Number::New(env, internal->stereoId), napi_enumerable));
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

    auto viewerTransform /** viewer to refspace(local) */ = XRSPACE_RELATIVE_TRANSFORM(viewerSpace, referenceSpace);
    auto viewerPoseObject = XRViewerPose::NewInstance(env, device, viewerTransform, internal, referenceSpace, session);
    return viewerPoseObject;
  }

  Napi::Value XRFrame::GetJointPose(const Napi::CallbackInfo &info)
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
      Napi::TypeError::New(env, "getJointPose requires a joint space and an XRSpace object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jointSpace = XRJointSpace::Unwrap(info[0].As<Napi::Object>());
    auto baseSpace = XRSpace::Unwrap(info[1].As<Napi::Object>());
    jointSpace->ensurePoseUpdated(id, session, internal);

    if (baseSpace->isReferenceSpace == true)
    {
      auto baseReferenceSpace = XRReferenceSpace::Unwrap(info[1].As<Napi::Object>());
      baseReferenceSpace->ensurePoseUpdated(id, session, internal);
      auto jointTransform /** joint to space(local) */ = XRSPACE_RELATIVE_TRANSFORM(jointSpace, baseReferenceSpace);
      return XRPose::NewInstance(env, device, jointTransform, internal);
    }
    else
    {
      Napi::TypeError::New(env, "getJointPose not support a non ReferenceSpace as `baseSpace`")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
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
    baseSpace->ensurePoseUpdated(id, session, internal);

    if (!space->isReferenceSpace && space->subType != XRSpaceSubType::UNSET)
    {
      auto inputSpace = XRTargetRayOrGripSpace::Unwrap(info[0].As<Napi::Object>());
      inputSpace->ensurePoseUpdated(id, session, internal);
      auto transform /** input source space to base(local/unbound) */ = XRSPACE_RELATIVE_TRANSFORM(inputSpace, baseSpace);
      return XRPose::NewInstance(env, device, transform, internal);
    }
    // TODO
    return env.Undefined();
  }

  uint32_t XRFrame::getStereoRenderingId()
  {
    return internal->stereoId;
  }

  void XRFrame::start()
  {
    active = true;
    animationFrame = true;
    device->startFrame(internal);
    startTime = chrono::high_resolution_clock::now();
  }

  void XRFrame::end()
  {
    active = false;
    device->endFrame(internal);
    endTime = chrono::high_resolution_clock::now();

    auto isMultipass = device->getDeviceInit().renderedAsMultipass();
    if (!isMultipass || internal->viewIndex == 0)
    {
      auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
      if (duration > 1000 / 60)
        fprintf(stderr, "Detected a long frame(#%d) takes %llums in session(%d)\n", id, duration, sessionId);
    }
  }
}
