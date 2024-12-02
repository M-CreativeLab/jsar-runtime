#include "client_context.hpp"
#include "common/xr/types.hpp"
#include "../webgl/rendering_context-inl.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *ClientContext::constructor;
  void ClientContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "ClientContext",
                                      {
                                          InstanceMethod("keepAlive", &ClientContext::KeepAlive),
                                          InstanceMethod("fastPerformanceNow", &ClientContext::FastPerformanceNow),
                                      });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("ClientContext", func);
  }

  ClientContext::ClientContext(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ClientContext>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    clientContext = TrClientContextPerProcess::Get();
    if (clientContext == nullptr)
    {
      Napi::TypeError::New(env, "Client context is not available.").ThrowAsJavaScriptException();
      return;
    }

    auto thisObject = info.This().ToObject();
    thisObject.Set("id", Napi::Number::New(env, clientContext->id));
    thisObject.Set("url", Napi::String::New(env, clientContext->url));
    thisObject.Set("applicationCacheDirectory",
                   Napi::String::New(env, clientContext->applicationCacheDirectory));
    thisObject.Set("httpsProxyServer", Napi::String::New(env, clientContext->httpsProxyServer));
    thisObject.Set("webglVersion", Napi::Number::New(env, clientContext->webglVersion));
    thisObject.Set("gl", clientContext->webglVersion == 2
                             ? ::webgl::WebGL2RenderingContext::MakeFromHost(env)
                             : ::webgl::WebGLRenderingContext::MakeFromHost(env));

    if (clientContext->xrDeviceInit.enabled == true)
    {
      auto xrDeviceObject = Napi::Object::New(env);
      xrDeviceObject.Set("enabled", Napi::Boolean::New(env, clientContext->xrDeviceInit.enabled));
      xrDeviceObject.Set("active", Napi::Boolean::New(env, clientContext->xrDeviceInit.active));
      auto mode = clientContext->xrDeviceInit.stereoRenderingMode;
      if (mode == xr::TrStereoRenderingMode::MultiPass)
        xrDeviceObject.Set("stereoRenderingMode", Napi::String::New(env, "multipass"));
      else if (mode == xr::TrStereoRenderingMode::SinglePass)
        xrDeviceObject.Set("stereoRenderingMode", Napi::String::New(env, "singlepass"));
      else if (mode == xr::TrStereoRenderingMode::SinglePassInstanced)
        xrDeviceObject.Set("stereoRenderingMode", Napi::String::New(env, "singlepassinstanced"));
      else if (mode == xr::TrStereoRenderingMode::SinglePassMultiview)
        xrDeviceObject.Set("stereoRenderingMode", Napi::String::New(env, "singlepassmultiview"));
      else
        xrDeviceObject.Set("stereoRenderingMode", Napi::String::New(env, "unknown"));
      thisObject.Set("xrDevice", xrDeviceObject);
    }
  }

  Napi::Value ClientContext::KeepAlive(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    clientContext->updateScriptTime();
    return env.Undefined();
  }

  Napi::Value ClientContext::FastPerformanceNow(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, clientContext->performanceNow());
  }
}
