#include "pose.hpp"
#include "./session.hpp"

namespace bindings
{
  Napi::FunctionReference *XRPose::constructor;
  Napi::FunctionReference *XRViewerPose::constructor;

  template <typename T>
  XRPoseBase<T>::XRPoseBase(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info),
                                                              transform(mat4(1.0f)),
                                                              emulatedPosition(false)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2 || !info[0].IsExternal() || !info[1].IsExternal())
    {
      Napi::TypeError::New(env, "XRPose constructor requires 2 arguments: XRPose([[native transform]], [[native frame]])").ThrowAsJavaScriptException();
      return;
    }

    auto transformExternal = info[0].As<Napi::External<mat4>>();
    transform = *transformExternal.Data();

    auto frameExternal = info[1].As<Napi::External<xr::TrXRFrameRequest>>();
    frameRequest = frameExternal.Data();
  }

  template <typename T>
  Napi::Value XRPoseBase<T>::TransformGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return XRRigidTransform::NewInstance(env, transform);
  }

  template <typename T>
  Napi::Value XRPoseBase<T>::EmulatedPositionGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, emulatedPosition);
  }

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

  Napi::Object XRPose::NewInstance(Napi::Env env, mat4 &transform, xr::TrXRFrameRequest *frameRequest)
  {
    Napi::EscapableHandleScope scope(env);
    auto transformExternal = Napi::External<mat4>::New(env, &transform);
    auto frameRequestExternal = Napi::External<xr::TrXRFrameRequest>::New(env, frameRequest);
    Napi::Object obj = constructor->New({transformExternal, frameRequestExternal});
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

  Napi::Object XRViewerPose::NewInstance(Napi::Env env, mat4 &transform, xr::TrXRFrameRequest *frameRequest, XRSession *xrSession)
  {
    Napi::EscapableHandleScope scope(env);
    auto transformExternal = Napi::External<mat4>::New(env, &transform);
    auto frameRequestExternal = Napi::External<xr::TrXRFrameRequest>::New(env, frameRequest);
    Napi::Object obj = constructor->New({transformExternal, frameRequestExternal, xrSession->Value()});
    return scope.Escape(obj).ToObject();
  }

  XRViewerPose::XRViewerPose(const Napi::CallbackInfo &info) : XRPoseBase(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3 || !info[2].IsObject() || info[2].ToObject().InstanceOf(XRSession::constructor->Value()) == false)
    {
      Napi::TypeError::New(env, "XRViewerPose() requires a `XRSession` object.").ThrowAsJavaScriptException();
      return;
    }
    auto xrSession = XRSession::Unwrap(info[2].ToObject());

    for (size_t viewIndex = 0; viewIndex < xr::TrXRFrameRequest::ViewsCount; viewIndex++)
    {
      xr::TrXRView &view = frameRequest->views[viewIndex];
      auto jsView = XRView::NewInstance(env, xrSession, view);
      views.push_back(Napi::Persistent(jsView));
    }
  }

  XRViewerPose::~XRViewerPose()
  {
    for (size_t i = 0; i < views.size(); i++)
      views[i].Unref();
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

  template class XRPoseBase<XRPose>;
  template class XRPoseBase<XRViewerPose>;
}
