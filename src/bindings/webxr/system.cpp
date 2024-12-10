#include <napi.h>
#include <client/per_process.hpp>
#include <client/xr/device.hpp>
#include "./system.hpp"
#include "./session.hpp"

namespace bindings::webxr
{
  using namespace std;

  thread_local Napi::FunctionReference *XRSystem::constructor;
  void XRSystem::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "XRSystem",
                                     {
                                         InstanceMethod("isSessionSupported", &XRSystem::IsSessionSupported, napi_default_method),
                                         InstanceMethod("requestSession", &XRSystem::RequestSession, napi_default_method),
                                     });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRSystem", tpl);
  }

  Napi::Object XRSystem::NewInstance(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    return scope.Escape(obj).ToObject();
  }

  XRSystem::XRSystem(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<XRSystem>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);

    uv_loop_t *jsEventloop;
    napi_get_uv_event_loop(env, &jsEventloop);
    handle_ = clientContext->getXRDeviceClient()->getXRSystem(jsEventloop);
  }

  Napi::Value XRSystem::IsSessionSupported(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    bool supported = false;
    if (info.Length() >= 1 && info[0].IsString())
    {
      client_xr::XRSessionMode mode = client_xr::XRSessionMode::Unknown;
      auto modeString = info[0].As<Napi::String>().Utf8Value();
      if (modeString == "inline")
        mode = client_xr::XRSessionMode::Inline;
      else if (modeString == "immersive-vr")
        mode = client_xr::XRSessionMode::ImmersiveVR;
      else if (modeString == "immersive-ar")
        mode = client_xr::XRSessionMode::ImmersiveAR;

      if (mode != client_xr::XRSessionMode::Unknown)
        supported = handle_->isSessionSupported(mode);
    }

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    deferred.Resolve(Napi::Boolean::New(env, supported));
    return deferred.Promise();
  }

  Napi::Value XRSystem::RequestSession(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Value modeValue;
    client_xr::XRSessionMode mode = client_xr::XRSessionMode::Unknown;

    if (info.Length() <= 0)
      modeValue = env.Undefined();
    else
      modeValue = info[0];

    auto modeString = modeValue.ToString().Utf8Value();
    if (modeString == "inline")
      mode = client_xr::XRSessionMode::Inline;
    else if (modeString == "immersive-vr")
      mode = client_xr::XRSessionMode::ImmersiveVR;
    else if (modeString == "immersive-ar")
      mode = client_xr::XRSessionMode::ImmersiveAR;
    else
    {
      auto msg = "The provided value '" + modeString + "' is not a valid enum value of type XRSessionMode";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
    }

    optional<client_xr::XRSessionRequestInit> requestInit = nullopt;
    if (info.Length() > 1 && info[1].IsObject())
    {
      // TODO
    }

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    auto newSession = XRSession::NewInstance(env, handle_->requestSession(mode, requestInit));
    deferred.Resolve(newSession);
    return deferred.Promise();
  }
}
