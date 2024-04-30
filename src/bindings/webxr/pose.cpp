#include "pose.hpp"

namespace bindings
{
  Napi::FunctionReference *XRPose::constructor;
  Napi::FunctionReference *XRViewerPose::constructor;

  template <typename T>
  XRPoseBase<T>::XRPoseBase(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info),
                                                              transform(mat4(1.0f)),
                                                              emulatedPosition(false)
  {
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

  Napi::Object XRPose::NewInstance(Napi::Env env, mat4 &transform)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    XRPose *instance = XRPose::Unwrap(obj);
    instance->transform = transform;
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

  Napi::Object XRViewerPose::NewInstance(Napi::Env env, mat4 &transform)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    XRViewerPose *instance = XRViewerPose::Unwrap(obj);
    instance->transform = transform;
    return scope.Escape(obj).ToObject();
  }

  XRViewerPose::XRViewerPose(const Napi::CallbackInfo &info) : XRPoseBase(info)
  {
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

  void XRViewerPose::addView(XRView *view)
  {
    addView(view->Value());
  }

  inline void XRViewerPose::addView(Napi::Object view)
  {
    views.push_back(Napi::Persistent(view));
  }

  template class XRPoseBase<XRPose>;
  template class XRPoseBase<XRViewerPose>;
}
