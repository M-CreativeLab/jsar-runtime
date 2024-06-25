#include "view.hpp"
#include "session.hpp"

namespace bindings
{
  Napi::FunctionReference *XRView::constructor;

  Napi::Object XRView::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRView",
                                     {
                                         InstanceMethod("requestViewportScale", &XRView::RequestViewportScale),
                                     });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRView", tpl);
    return exports;
  }

  Napi::Object XRView::NewInstance(Napi::Env env, XRSession *session, xr::TrXRView &view)
  {
    Napi::EscapableHandleScope scope(env);
    auto viewMatrixTransform = XRRigidTransform::NewInstance(env, glm::inverse(view.getViewMatrix()));
    auto projectionMatrixTransform = XRRigidTransform::NewInstance(env, view.getProjectionMatrix());
    auto viewIndex = Napi::Number::New(env, view.viewIndex);
    auto viewportExternal = Napi::External<TrViewport>::New(env, &view.viewport);

    Napi::Object obj = constructor->New({session->Value(),
                                         viewMatrixTransform,
                                         projectionMatrixTransform,
                                         viewIndex,
                                         viewportExternal});
    return scope.Escape(obj).ToObject();
  }

  XRView::XRView(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRView>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "Invalid arguments to construct XRView(session, viewTransform, projectionTransform, viewIndex, viewport).")
          .ThrowAsJavaScriptException();
      return;
    }
    if (!info[4].IsExternal())
    {
      Napi::TypeError::New(env, "XRView constructor requires a TrViewport external object.").ThrowAsJavaScriptException();
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
    viewport = *info[4].As<Napi::External<TrViewport>>().Data();

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("eye", InitEye(env), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("projectionMatrix", InitProjectionMatrix(env), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("transform", XRRigidTransform::NewInstance(env, transformMatrix), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("recommendedViewportScale", env.Null(), napi_enumerable));
  }

  Napi::Value XRView::InitEye(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::String jsEye;
    if (index == 0)
      jsEye = Napi::String::New(env, "left");
    else if (index == 1)
      jsEye = Napi::String::New(env, "right");
    else
      jsEye = Napi::String::New(env, "none");
    return scope.Escape(jsEye);
  }

  Napi::Value XRView::InitProjectionMatrix(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    auto float32arrayValue = Napi::Float32Array::New(env, 16);
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        float32arrayValue.Set(static_cast<uint32_t>(i * 4 + j), Napi::Number::New(env, projectionMatrix[i][j]));
      }
    }
    return scope.Escape(float32arrayValue);
  }

  Napi::Value XRView::RequestViewportScale(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  TrViewport XRView::getViewport()
  {
    return device->getViewport(index);
  }
}
