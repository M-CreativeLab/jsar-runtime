#include "device_native.hpp"
#include "common.hpp"

namespace bindings
{
  Napi::FunctionReference *XRDeviceNative::constructor;

  struct XRFrameRequestContext
  {
    XRDeviceNative *device;
    xr::TrXRFrameRequest *frameRequest;
  };

  Napi::Object XRDeviceNative::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRDeviceNative",
                                     {InstanceMethod("isSessionSupported", &XRDeviceNative::IsSessionSupported),
                                      InstanceMethod("requestSession", &XRDeviceNative::RequestSession)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("XRDeviceNative", tpl);
    return exports;
  }

  XRDeviceNative *XRDeviceNative::GetInstance()
  {
    return nullptr;
  }

  XRDeviceNative::XRDeviceNative(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRDeviceNative>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);

    frameHandler = new Napi::FunctionReference();
    *frameHandler = Napi::Persistent(Napi::Function::New(env, NativeFrameHandler));
    tsfnWithFrameHandler = Napi::ThreadSafeFunction::New(env, frameHandler->Value(), "XRDeviceNative::FrameHandler", 0, 1);

    // Register frame request handler
    // FIXME: should we move this to startSession?
    clientContext->requestFrame(frame_request::TrFrameRequestType::XRFrame, [this](frame_request::TrFrameRequestMessage &msg)
                                {
                                  auto xrFrameReq = xr::TrXRFrameRequest::MakeFromMessage<xr::TrXRFrameRequest>(msg);
                                  if (xrFrameReq != nullptr)
                                  {
                                    this->handleFrameRequest(xrFrameReq);
                                  } });
  }

  Napi::Value XRDeviceNative::IsSessionSupported(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: expected 1 argument")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "mode should be a string")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto modeString = info[0].As<Napi::String>().Utf8Value();
    xr::TrXRSessionMode mode = xr::MakeSessionMode(modeString);
    if (mode == xr::TrXRSessionMode::Unknown)
    {
      Napi::TypeError::New(env, "mode should be 'immersive-ar', 'immersive-vr' or 'inline'.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    xr::IsSessionSupportedRequest request(mode);
    if (!clientContext->sendXrCommand(request))
    {
      Napi::TypeError::New(env, "XRDeviceNative::IsSessionSupported: failed to send command")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto resp = clientContext->recvXrCommand<xr::SessionResponse>(xr::TrXRCmdType::SessionResponse, 1000);
    if (resp == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::IsSessionSupported: failed to receive response")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto isSupportedValue = Napi::Boolean::New(env, resp->success);
    delete resp;
    deferred.Resolve(isSupportedValue);
    return deferred.Promise();
  }

  Napi::Value XRDeviceNative::RequestSession(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: expected 1 argument")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "mode should be a string")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    xr::TrXRSessionMode mode;
    auto modeString = info[0].As<Napi::String>().Utf8Value();
    mode = xr::MakeSessionMode(modeString);
    if (mode == xr::TrXRSessionMode::Unknown)
    {
      Napi::TypeError::New(env, "mode should be 'immersive-ar', 'immersive-vr' or 'inline'.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    xr::SessionRequest request(mode);
    if (!clientContext->sendXrCommand(request))
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: failed to send command")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto resp = clientContext->recvXrCommand<xr::SessionResponse>(xr::TrXRCmdType::SessionResponse, 1000);
    if (resp == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: failed to receive response")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto sessionIdValue = Napi::Number::New(env, resp->sessionId);
    delete resp;
    deferred.Resolve(sessionIdValue);
    return deferred.Promise();
  }

  Napi::Value XRDeviceNative::NativeFrameHandler(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "expected an external.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto data = info[0].As<Napi::External<XRFrameRequestContext>>().Data();
    if (data == nullptr)
    {
      Napi::TypeError::New(env, "invalid external data.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto instance = data->device;
    auto frameRequest = data->frameRequest;
    instance->clientContext->setFramebufferWidth(frameRequest->framebufferWidth);
    instance->clientContext->setFramebufferHeight(frameRequest->framebufferHeight);

    auto contextifiedFrameCallbacks = instance->contextifiedFrameCallbacks;
    instance->contextifiedFrameCallbacks.clear(); // Clear the contextifiedFrameCallbacks

    for (auto &callbackWithContext : contextifiedFrameCallbacks)
      callbackWithContext.callback(env, frameRequest, callbackWithContext.context);

    // Clear the `XRFrameRequestContext` struct
    delete frameRequest;
    delete data;
    return env.Undefined();
  }

  bool XRDeviceNative::supportsSessionMode(XRSessionMode mode)
  {
    if (mode == XRSessionMode::IMMERSIVE_AR)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool XRDeviceNative::supportsReferenceSpaceType(XRReferenceSpaceType type)
  {
    if (
        type == XRReferenceSpaceType::LOCAL ||
        type == XRReferenceSpaceType::VIEWER ||
        type == XRReferenceSpaceType::UNBOUNDED)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  void XRDeviceNative::requestFrame(XRFrameCallback callback, void *context)
  {
    contextifiedFrameCallbacks.push_back(ContextifiedXRFrameCallback(callback, context));
  }

  bool XRDeviceNative::startFrame(xr::TrXRFrameRequest *frameRequest)
  {
    return clientContext->startXrFrame(frameRequest);
  }

  bool XRDeviceNative::endFrame(xr::TrXRFrameRequest *frameRequest)
  {
    return clientContext->finishXrFrame(frameRequest);
  }

  TrViewport XRDeviceNative::getViewport(uint32_t viewIndex)
  {
    // TODO
    return TrViewport();
  }

  xr::TrDeviceInit &XRDeviceNative::getDeviceInit()
  {
    return clientContext->xrDeviceInit;
  }

  void XRDeviceNative::handleFrameRequest(xr::TrXRFrameRequest *frameRequest)
  {
    auto data = new XRFrameRequestContext{this, frameRequest};
    tsfnWithFrameHandler.NonBlockingCall(data, [](Napi::Env env, Napi::Function jsCallback, XRFrameRequestContext *context)
                                         { jsCallback.Call({Napi::External<XRFrameRequestContext>::New(env, context)}); });
  }
}
