#include "viewport.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRViewport::constructor;

  // static
  void XRViewport::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(
      env,
      "XRViewport",
      {InstanceValue("width", Napi::Number::New(env, 0), napi_default_jsproperty),
       InstanceValue("height", Napi::Number::New(env, 0), napi_default_jsproperty),
       InstanceValue("x", Napi::Number::New(env, 0), napi_default_jsproperty),
       InstanceValue("y", Napi::Number::New(env, 0), napi_default_jsproperty)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRViewport", tpl);
  }

  Napi::Object XRViewport::NewInstance(Napi::Env env, TrViewport viewport)
  {
    return NewInstance(env, client_xr::XRViewport(viewport));
  }

  Napi::Object XRViewport::NewInstance(Napi::Env env, client_xr::XRViewport viewport)
  {
    Napi::EscapableHandleScope scope(env);
    auto jsViewport = Napi::External<client_xr::XRViewport>::New(env, &viewport);
    Napi::Object obj = constructor->New({jsViewport});
    return scope.Escape(obj).ToObject();
  }

  XRViewport::XRViewport(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<XRViewport>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 && info[0].IsExternal())
    {
      config = *static_cast<client_xr::XRViewport *>(info[0].As<Napi::External<client_xr::XRViewport>>().Data());
    }
    else if (info.Length() == 4)
    {
      config = client_xr::XRViewport(info[0].As<Napi::Number>().Uint32Value(),
                                     info[1].As<Napi::Number>().Uint32Value(),
                                     info[2].As<Napi::Number>().Uint32Value(),
                                     info[3].As<Napi::Number>().Uint32Value());
    }
    else
    {
      Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
      return;
    }

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.Set("width", Napi::Number::New(env, config.width));
    jsThis.Set("height", Napi::Number::New(env, config.height));
    jsThis.Set("x", Napi::Number::New(env, config.x));
    jsThis.Set("y", Napi::Number::New(env, config.y));
  }
}
