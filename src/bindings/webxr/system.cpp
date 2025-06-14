#include <napi.h>
#include <client/per_process.hpp>
#include <client/xr/device.hpp>
#include "./system.hpp"
#include "./session.hpp"

using namespace std;
using namespace Napi;

namespace bindings::webxr
{
  thread_local FunctionReference *XRSystem::constructor;
  void XRSystem::Init(Napi::Env env)
  {
#define MODULE_NAME "XRSystem"
    Function tpl = DefineClass(
      env,
      MODULE_NAME,
      {
        InstanceMethod("isSessionSupported", &XRSystem::IsSessionSupported, napi_default_method),
        InstanceMethod("requestSession", &XRSystem::RequestSession, napi_default_method),
      });

    constructor = new FunctionReference();
    *constructor = Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set(MODULE_NAME, tpl);
#undef MODULE_NAME
  }

  Object XRSystem::NewInstance(Napi::Env env)
  {
    EscapableHandleScope scope(env);
    Object obj = constructor->New({});
    return scope.Escape(obj).ToObject();
  }

  XRSystem::XRSystem(const CallbackInfo &info)
      : ObjectWrap<XRSystem>(info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    auto clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);

    uv_loop_t *jsEventloop;
    napi_get_uv_event_loop(env, &jsEventloop);
    handle_ = clientContext->getXRDeviceClient()->getXRSystem(jsEventloop);
  }

  Value XRSystem::IsSessionSupported(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    bool supported = false;
    if (info.Length() >= 1 && info[0].IsString())
    {
      client_xr::XRSessionMode mode = client_xr::XRSessionMode::Unknown;
      auto modeString = info[0].As<String>().Utf8Value();
      if (modeString == "inline")
        mode = client_xr::XRSessionMode::Inline;
      else if (modeString == "immersive-vr")
        mode = client_xr::XRSessionMode::ImmersiveVR;
      else if (modeString == "immersive-ar")
        mode = client_xr::XRSessionMode::ImmersiveAR;

      if (mode != client_xr::XRSessionMode::Unknown)
        supported = handle_->isSessionSupported(mode);
    }

    Promise::Deferred deferred = Promise::Deferred::New(env);
    deferred.Resolve(Napi::Boolean::New(env, supported));
    return deferred.Promise();
  }

  Value XRSystem::RequestSession(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

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
      TypeError::New(env, msg).ThrowAsJavaScriptException();
    }

    optional<client_xr::XRSessionRequestInit> requestInit = nullopt;
    if (info.Length() > 1 && info[1].IsObject())
    {
      // TODO
    }

    Promise::Deferred deferred = Promise::Deferred::New(env);
    auto newSession = XRSession::NewInstance(env, handle_->requestSession(mode, requestInit));
    deferred.Resolve(newSession);
    return deferred.Promise();
  }
}
