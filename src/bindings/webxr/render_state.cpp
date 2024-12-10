#include "render_state.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRRenderState::constructor;
  Napi::Object XRRenderState::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRRenderState",
                                     {InstanceAccessor("baseLayer", &XRRenderState::BaseLayerGetter, nullptr),
                                      InstanceAccessor("depthFar", &XRRenderState::DepthFarGetter, nullptr),
                                      InstanceAccessor("depthNear", &XRRenderState::DepthNearGetter, nullptr),
                                      InstanceAccessor("inlineVerticalFieldOfView", &XRRenderState::InlineVerticalFieldOfViewGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRRenderState", tpl);
    return exports;
  }

  Napi::Object XRRenderState::NewInstance(Napi::Env env, client_xr::XRRenderState state)
  {
    Napi::EscapableHandleScope scope(env);

    auto stateExternal = Napi::External<client_xr::XRRenderState>::New(env, &state);
    Napi::Object instance = constructor->New({stateExternal});
    return scope.Escape(instance).ToObject();
  }

  XRRenderState::XRRenderState(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRRenderState>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<Napi::External<client_xr::XRRenderState>>();
    handle_ = *external.Data();
  }

  Napi::Value XRRenderState::BaseLayerGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return XRWebGLLayer::NewInstance(env, handle_.baseLayer);
  }

  Napi::Value XRRenderState::DepthFarGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, handle_.depthFar);
  }

  Napi::Value XRRenderState::DepthNearGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, handle_.depthNear);
  }

  Napi::Value XRRenderState::InlineVerticalFieldOfViewGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, handle_.inlineVerticalFieldOfView);
  }
}
