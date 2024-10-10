#include "pose.hpp"
#include "./session.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRPose::constructor;
  thread_local Napi::FunctionReference *XRViewerPose::constructor;

  Napi::Object XRPose::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRPose",
                                     {InstanceAccessor("transform", &XRPose::TransformGetter, nullptr),
                                      InstanceAccessor("emulatedPosition", &XRPose::EmulatedPositionGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRPose", tpl);
    return exports;
  }

  Napi::Object XRPose::NewInstance(Napi::Env env, XRDeviceNative *device, mat4 &transform, xr::TrXRFrameRequest *frameRequest)
  {
    Napi::EscapableHandleScope scope(env);
    auto deviceNativeExternal = Napi::External<XRDeviceNative>::New(env, device);
    auto transformExternal = Napi::External<mat4>::New(env, &transform);
    auto frameRequestExternal = Napi::External<xr::TrXRFrameRequest>::New(env, frameRequest);
    Napi::Object obj = constructor->New({deviceNativeExternal, transformExternal, frameRequestExternal});
    return scope.Escape(obj).ToObject();
  }

  XRPose::XRPose(const Napi::CallbackInfo &info) : XRPoseBase(info)
  {
  }

  Napi::Object XRViewerPose::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRViewerPose",
                                     {InstanceAccessor("transform", &XRViewerPose::TransformGetter, nullptr),
                                      InstanceAccessor("emulatedPosition", &XRViewerPose::EmulatedPositionGetter, nullptr),
                                      InstanceAccessor("views", &XRViewerPose::ViewsGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRViewerPose", tpl);
    return exports;
  }

  Napi::Object XRViewerPose::NewInstance(Napi::Env env, XRDeviceNative *device, mat4 &transform, xr::TrXRFrameRequest *frameRequest, XRReferenceSpace *baseSpace, XRSession *xrSession)
  {
    Napi::EscapableHandleScope scope(env);
    auto deviceNativeExternal = Napi::External<XRDeviceNative>::New(env, device);
    auto transformExternal = Napi::External<mat4>::New(env, &transform);
    auto frameRequestExternal = Napi::External<xr::TrXRFrameRequest>::New(env, frameRequest);
    Napi::Object obj = constructor->New({deviceNativeExternal,
                                         transformExternal,
                                         frameRequestExternal,
                                         baseSpace->Value(),
                                         xrSession->Value()});
    return scope.Escape(obj).ToObject();
  }

  XRViewerPose::XRViewerPose(const Napi::CallbackInfo &info) : XRPoseBase(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "XRViewerPose() requires 5 arguments to construct.")
          .ThrowAsJavaScriptException();
      return;
    }
    if (!info[3].IsObject() || !info[3].ToObject().InstanceOf(XRReferenceSpace::constructor->Value()))
    {
      Napi::TypeError::New(env, "Invalid argument to construct XRViewerPose(): not a `XRReferenceSpace` object.")
          .ThrowAsJavaScriptException();
      return;
    }
    if (!info[4].IsObject() || !info[4].ToObject().InstanceOf(XRSession::constructor->Value()))
    {
      Napi::TypeError::New(env, "Invalid argument to construct XRViewerPose(): not a `XRSession` object.")
          .ThrowAsJavaScriptException();
      return;
    }

    auto baseReferenceSpace = XRReferenceSpace::Unwrap(info[3].ToObject());
    auto xrSession = XRSession::Unwrap(info[4].ToObject());

    /**
     * Create views.
     *
     * If the device is rendered as multipass, only one view is created, and the view is for the current eye.
     * Otherwise, 2 views are created, one for each eye.
     */
    if (device->getDeviceInit().renderedAsMultipass())
    {
      xr::TrXRView &view = frameRequest->views[frameRequest->viewIndex];
      auto jsView = XRView::NewInstance(env, xrSession, view, baseReferenceSpace);
      views.push_back(Napi::Persistent(jsView));
    }
    else
    {
      for (size_t viewIndex = 0; viewIndex < xr::TrXRFrameRequest::ViewsCount; viewIndex++)
      {
        xr::TrXRView &view = frameRequest->views[viewIndex];
        auto jsView = XRView::NewInstance(env, xrSession, view, baseReferenceSpace);
        views.push_back(Napi::Persistent(jsView));
      }
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
}
