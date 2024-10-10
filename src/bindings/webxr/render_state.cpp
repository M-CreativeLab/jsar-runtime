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

  Napi::Object XRRenderState::NewInstance(Napi::Env env, xr::RenderState state)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    XRRenderState *instance = XRRenderState::Unwrap(obj);
    instance->state = state;
    return scope.Escape(obj).ToObject();
  }

  XRRenderState::XRRenderState(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRRenderState>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  Napi::Value XRRenderState::BaseLayerGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return XRWebGLLayer::NewInstance(env, *state.baseLayer);
  }

  Napi::Value XRRenderState::DepthFarGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, state.depthFar);
  }

  Napi::Value XRRenderState::DepthNearGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, state.depthNear);
  }

  Napi::Value XRRenderState::InlineVerticalFieldOfViewGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, state.inlineVerticalFieldOfView);
  }
}
