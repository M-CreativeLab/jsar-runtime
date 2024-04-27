#include "viewport.hpp"

namespace bindings
{
  Napi::FunctionReference *XRViewport::constructor;

  Napi::Object XRViewport::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRViewport",
                                     {InstanceAccessor("width", &XRViewport::WidthGetter, nullptr),
                                      InstanceAccessor("height", &XRViewport::HeightGetter, nullptr),
                                      InstanceAccessor("x", &XRViewport::XAxisGetter, nullptr),
                                      InstanceAccessor("y", &XRViewport::YAxisGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRViewport", tpl);
    return exports;
  }

  Napi::Object XRViewport::NewInstance(Napi::Env env, xr::Viewport viewport)
  {
    Napi::EscapableHandleScope scope(env);
    auto jsViewport = Napi::External<xr::Viewport>::New(env, &viewport);
    Napi::Object obj = constructor->New({jsViewport});
    return scope.Escape(obj).ToObject();
  }

  XRViewport::XRViewport(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRViewport>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 && info[0].IsExternal())
    {
      config = *static_cast<xr::Viewport *>(info[0].As<Napi::External<xr::Viewport>>().Data());
    }
    else if (info.Length() == 4)
    {
      config = xr::Viewport(info[0].As<Napi::Number>().Uint32Value(),
                            info[1].As<Napi::Number>().Uint32Value(),
                            info[2].As<Napi::Number>().Uint32Value(),
                            info[3].As<Napi::Number>().Uint32Value());
    }
    else
    {
      Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
    }
  }

  Napi::Value XRViewport::WidthGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.width);
  }

  Napi::Value XRViewport::HeightGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.height);
  }

  Napi::Value XRViewport::XAxisGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.x);
  }

  Napi::Value XRViewport::YAxisGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.y);
  }

}