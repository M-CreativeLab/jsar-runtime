#include "view.hpp"
#include "session.hpp"

namespace bindings
{
  Napi::FunctionReference *XRView::constructor;

  Napi::Object XRView::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRView",
                                     {
                                         InstanceAccessor("eye", &XRView::EyeGetter, nullptr),
                                         InstanceAccessor("projectionMatrix", &XRView::ProjectionMatrixGetter, nullptr),
                                         InstanceAccessor("transform", &XRView::TransformGetter, nullptr),
                                         InstanceAccessor("recommendedViewportScale", &XRView::RecommendedViewportScaleGetter, nullptr),
                                         InstanceMethod("requestViewportScale", &XRView::RequestViewportScale),
                                     });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRView", tpl);
    return exports;
  }

  Napi::Object XRView::NewInstance(Napi::Env env,
                                   XRSession *session,
                                   mat4 &viewMatrix,
                                   mat4 &projectionMatrix,
                                   uint32_t index,
                                   XREye eye)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({session->Value(),
                                         XRRigidTransform::NewInstance(env, viewMatrix),
                                         XRRigidTransform::NewInstance(env, projectionMatrix),
                                         Napi::Number::New(env, index),
                                         Napi::Number::New(env, static_cast<uint32_t>(eye))});
    return scope.Escape(obj).ToObject();
  }

  XRView::XRView(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRView>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "XRView constructor requires a session object, view matrix, projection matrix, index, and eye")
          .ThrowAsJavaScriptException();
      return;
    }

    Napi::Object sessionObj = info[0].As<Napi::Object>();
    auto session = XRSession::Unwrap(sessionObj);
    sessionId = session->id;
    device = session->device;

    Napi::Object transformObj = info[1].As<Napi::Object>();
    auto transform = XRRigidTransform::Unwrap(transformObj);
    this->transformMatrix = transform->matrix;

    Napi::Object projectionObj = info[2].As<Napi::Object>();
    auto projection = XRRigidTransform::Unwrap(projectionObj);
    this->projectionMatrix = projection->matrix;

    index = info[3].As<Napi::Number>().Uint32Value();
    eyeId = info[4].As<Napi::Number>().Uint32Value();
    viewport = device->getViewport(index);
  }

  Napi::Value XRView::EyeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (eyeId == 0)
      return Napi::String::New(env, "left");
    else if (eyeId == 1)
      return Napi::String::New(env, "right");
    else
      return Napi::String::New(env, "none");
  }

  Napi::Value XRView::ProjectionMatrixGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto float32arrayValue = Napi::Float32Array::New(env, 16);
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        float32arrayValue.Set(static_cast<uint32_t>(i * 4 + j), Napi::Number::New(env, projectionMatrix[i][j]));
      }
    }
    return float32arrayValue;
  }

  Napi::Value XRView::TransformGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return XRRigidTransform::NewInstance(env, transformMatrix);
  }

  Napi::Value XRView::RecommendedViewportScaleGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value XRView::RequestViewportScale(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  xr::Viewport XRView::getViewport()
  {
    viewport = device->getViewport(index);
    return viewport;
  }
}
