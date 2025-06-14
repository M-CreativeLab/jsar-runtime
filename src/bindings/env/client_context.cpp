#include "client_context.hpp"
#include "common/xr/types.hpp"
#include "../webgl/rendering_context-inl.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *ClientContext::constructor;
  void ClientContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(
      env,
      "ClientContext",
      {
        InstanceMethod("keepAlive", &ClientContext::KeepAlive),
        InstanceMethod("fastPerformanceNow", &ClientContext::FastPerformanceNow),
      });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("ClientContext", func);
  }

  ClientContext::ClientContext(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ClientContext>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    clientContext = TrClientContextPerProcess::Get();
    if (clientContext == nullptr)
    {
      Napi::TypeError::New(env, "Client context is not available.").ThrowAsJavaScriptException();
      return;
    }
    if (info.Length() < 1 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "Failed to construct 'ClientContext': missing init object.").ThrowAsJavaScriptException();
      return;
    }

    auto initObject = info[0].ToObject();
    bool isWorker = false;
    if (initObject.Has("isWorker"))
      isWorker = initObject.Get("isWorker").ToBoolean();

    auto thisObject = info.This().ToObject();
    thisObject.Set("id", Napi::Number::New(env, clientContext->id));
    thisObject.Set("url", Napi::String::New(env, clientContext->url));
    thisObject.Set("applicationCacheDirectory",
                   Napi::String::New(env, clientContext->applicationCacheDirectory));
    thisObject.Set("httpsProxyServer", Napi::String::New(env, clientContext->httpsProxyServer));
    thisObject.Set("webglVersion", Napi::Number::New(env, clientContext->webglVersion));

    // Disable the host webgl context for worker threads
    if (!isWorker)
    {
      thisObject.Set(
        "gl",
        clientContext->webglVersion == 2
          ? ::webgl::WebGL2RenderingContext::MakeFromHost(env)
          : ::webgl::WebGLRenderingContext::MakeFromHost(env));
    }

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

    if (!isWorker)
    {
      /**
       * Configure the scripting event loop to enable the native rendering.
       *
       * The native rendering such as "builtin_scene" requires the scripting's event loop to be set, and then the renderer can use
       * the event loop to schedule the rendering tasks.
       */
      clientContext->setScriptingEventLoop(env);
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
