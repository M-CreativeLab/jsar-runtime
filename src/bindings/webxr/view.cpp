#include "view.hpp"
#include "session.hpp"

namespace bindings
{
  using namespace std;

  thread_local Napi::FunctionReference *XRView::constructor;

  // static
  void XRView::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(
      env,
      "XRView",
      {
        InstanceMethod("requestViewportScale", &XRView::RequestViewportScale),
      });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRView", tpl);
  }

  // static
  Napi::Object XRView::NewInstance(Napi::Env env, shared_ptr<client_xr::XRView> handle)
  {
    Napi::EscapableHandleScope scope(env);
    SharedReference<client_xr::XRView> handleRef(handle);
    auto handleExternal = Napi::External<SharedReference<client_xr::XRView>>::New(env, &handleRef);
    Napi::Object instance = XRView::constructor->New({handleExternal});
    return scope.Escape(instance).ToObject();
  }

  XRView::XRView(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<XRView>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto handleExternal = info[0].As<Napi::External<SharedReference<client_xr::XRView>>>();
    auto handleRef = handleExternal.Data();
    if (!handleRef)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
    handle_ = handleRef->value;

    // Define properties
    auto jsThis = info.This().As<Napi::Object>();
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("eye", InitEye(env), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("projectionMatrix", InitProjectionMatrix(env), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("transform",
                                                          XRRigidTransform::NewInstance(env, handle_->transform()),
                                                          napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("recommendedViewportScale", env.Null(), napi_enumerable));
  }

  Napi::Value XRView::InitEye(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::String jsEye;

    auto eye = handle_->eye();
    if (eye == client_xr::XREye::kLeft)
      jsEye = Napi::String::New(env, "left");
    else if (eye == client_xr::XREye::kRight)
      jsEye = Napi::String::New(env, "right");
    else
      jsEye = Napi::String::New(env, "none");
    return scope.Escape(jsEye);
  }

  Napi::Value XRView::InitProjectionMatrix(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    auto projectionMatrix = handle_->projectionMatrix();
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

    float scale = 1.0f;
    if (info.Length() > 0)
      scale = info[0].ToNumber().FloatValue();

    try
    {
      handle_->requestViewportScale(scale);
    }
    catch (const std::exception &e)
    {
      Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  client_xr::XRViewport XRView::getViewport()
  {
    return handle_->viewport();
  }
}
