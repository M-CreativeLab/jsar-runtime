#include "render_state.hpp"

namespace bindings
{
  using namespace std;
  using namespace Napi;

  thread_local FunctionReference *XRRenderState::constructor;

  // static
  void XRRenderState::Init(Napi::Env env)
  {
    Function tpl = DefineClass(env, "XRRenderState",
                               {InstanceAccessor("baseLayer", &XRRenderState::BaseLayerGetter, nullptr),
                                InstanceAccessor("depthFar", &XRRenderState::DepthFarGetter, nullptr),
                                InstanceAccessor("depthNear", &XRRenderState::DepthNearGetter, nullptr),
                                InstanceAccessor("inlineVerticalFieldOfView", &XRRenderState::InlineVerticalFieldOfViewGetter, nullptr)});

    constructor = new FunctionReference();
    *constructor = Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRRenderState", tpl);
  }

  Object XRRenderState::NewInstance(Napi::Env env, client_xr::XRRenderState state)
  {
    EscapableHandleScope scope(env);

    auto stateExternal = External<client_xr::XRRenderState>::New(env, &state);
    Object instance = constructor->New({stateExternal});
    return scope.Escape(instance).ToObject();
  }

  XRRenderState::XRRenderState(const CallbackInfo &info) : ObjectWrap<XRRenderState>(info)
  {
    auto env = info.Env();
    HandleScope scope(info.Env());

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<External<client_xr::XRRenderState>>();
    handle_ = *external.Data();
  }

  Napi::Value XRRenderState::BaseLayerGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (handle_.baseLayer == nullptr)
      return env.Null();
    else
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
