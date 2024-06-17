#include "device_native.hpp"
#include "common.hpp"
#include "xr/device.hpp"

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
                                      InstanceMethod("requestSession", &XRDeviceNative::RequestSession),
                                      InstanceMethod("requestFrameOfReferenceTransform", &XRDeviceNative::RequestFrameOfReferenceTransform),
                                      InstanceMethod("getViewport", &XRDeviceNative::GetViewport),
                                      InstanceMethod("getViewerTransform", &XRDeviceNative::GetViewerTransform),
                                      InstanceMethod("getViewerStereoViewMatrix", &XRDeviceNative::GetViewerStereoViewMatrix),
                                      InstanceMethod("getViewerStereoProjectionMatrix", &XRDeviceNative::GetViewerStereoProjectionMatrix),
                                      InstanceMethod("getActiveEyeId", &XRDeviceNative::GetActiveEyeId),
                                      InstanceMethod("getGazeInputSource", &XRDeviceNative::GetGazeInputSource),
                                      InstanceMethod("getHandInputSource", &XRDeviceNative::GetHandInputSource),
                                      InstanceMethod("getGamepadInputSources", &XRDeviceNative::GetGamepadInputSources),
                                      InstanceMethod("getScreenInputSources", &XRDeviceNative::GetScreenInputSources),
                                      InstanceMethod("startFrame", &XRDeviceNative::StartFrame),
                                      InstanceMethod("endFrame", &XRDeviceNative::EndFrame)});

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

    xr::TrSessionMode mode;
    auto modeString = info[0].As<Napi::String>().Utf8Value();
    if (modeString == "immersive-ar")
    {
      mode = xr::TrSessionMode::ImmersiveAR;
    }
    else if (modeString == "immersive-vr")
    {
      mode = xr::TrSessionMode::ImmersiveVR;
    }
    else if (modeString == "inline")
    {
      mode = xr::TrSessionMode::Inline;
    }
    else
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

    xr::TrSessionMode mode;
    auto modeString = info[0].As<Napi::String>().Utf8Value();
    if (modeString == "immersive-ar")
    {
      mode = xr::TrSessionMode::ImmersiveAR;
    }
    else if (modeString == "immersive-vr")
    {
      mode = xr::TrSessionMode::ImmersiveVR;
    }
    else if (modeString == "inline")
    {
      mode = xr::TrSessionMode::Inline;
    }
    else
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

  Napi::Value XRDeviceNative::RequestFrameOfReferenceTransform(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: expected 2 arguments")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: sessionId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: type must be a string")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto id = info[0].As<Napi::Number>().Int32Value();
    auto typeStr = info[1].As<Napi::String>().Utf8Value();
    if (typeStr != "local" && typeStr != "viewer")
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: expected 'local' or 'viewer'")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (typeStr == "local")
    {
      auto transform = device->getLocalTransform(id);
      if (transform != nullptr)
      {
        auto array = Napi::Float32Array::New(env, 16);
        for (int i = 0; i < 16; i++)
          array[i] = Napi::Number::New(env, transform[i]);
        return array;
      }
    }
    else if (typeStr == "viewer")
    {
      auto transform = device->getViewerTransform();
      if (transform != nullptr)
      {
        auto array = Napi::Float32Array::New(env, 16);
        for (int i = 0; i < 16; i++)
          array[i] = Napi::Number::New(env, transform[i]);
        return array;
      }
    }
    return env.Null();
  }

  Napi::Value XRDeviceNative::GetViewport(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewport: expected 3 arguments")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewport: sessionId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewport: eye must be a string \"left\" or \"right\"")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewport: viewId or passId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // auto sessionId = info[0].As<Napi::Number>().Int32Value(); // not used
    auto viewOrPassId = info[2].As<Napi::Number>().Int32Value();

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewport: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto viewport = device->getViewport(viewOrPassId);
    auto jsViewport = Napi::Object::New(env);
    jsViewport.Set("x", Napi::Number::New(env, viewport.x));
    jsViewport.Set("y", Napi::Number::New(env, viewport.y));
    jsViewport.Set("width", Napi::Number::New(env, viewport.width));
    jsViewport.Set("height", Napi::Number::New(env, viewport.height));
    return jsViewport;
  }

  Napi::Value XRDeviceNative::GetViewerTransform(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto viewerTransform = device->getViewerTransform();
    if (viewerTransform != nullptr)
    {
      auto array = Napi::Float32Array::New(env, 16);
      for (int i = 0; i < 16; i++)
        array[i] = Napi::Number::New(env, viewerTransform[i]);
      return array;
    }
    else
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: viewer transform is not available")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  Napi::Value XRDeviceNative::GetViewerStereoViewMatrix(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoViewMatrix: expected a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto eyeId = info[0].As<Napi::Number>().Int32Value();
    if (eyeId != 0 && eyeId != 1)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoViewMatrix: expected 0(left) or 1(right)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoViewMatrix: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto viewMatrix = device->getViewerStereoViewMatrix(eyeId);
    if (viewMatrix != nullptr)
    {
      auto array = Napi::Float32Array::New(env, 16);
      for (int i = 0; i < 16; i++)
        array[i] = Napi::Number::New(env, viewMatrix[i]);
      return array;
    }
    else
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoViewMatrix: viewer stereo view matrix is not available")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  Napi::Value XRDeviceNative::GetViewerStereoProjectionMatrix(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoProjectionMatrix: expected a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto eyeId = info[0].As<Napi::Number>().Int32Value();
    if (eyeId != 0 && eyeId != 1)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoProjectionMatrix: expected 0(left) or 1(right)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoProjectionMatrix: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto projectionMatrix = device->getViewerStereoProjectionMatrix(eyeId);
    if (projectionMatrix != nullptr)
    {
      auto array = Napi::Float32Array::New(env, 16);
      for (int i = 0; i < 16; i++)
        array[i] = Napi::Number::New(env, projectionMatrix[i]);
      return array;
    }
    else
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetViewerStereoProjectionMatrix: viewer stereo projection matrix is not available")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  Napi::Value XRDeviceNative::GetActiveEyeId(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetActiveEyeId: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    return Napi::Number::New(env, device->getActiveEyeId());
  }

  Napi::Value XRDeviceNative::GetGazeInputSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::GetGazeInputSource: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto gazeInputSource = device->getGazeInputSource();
    auto jsInputSource = Napi::Object::New(env);
    jsInputSource.Set("id", Napi::Number::New(env, gazeInputSource->id));
    jsInputSource.Set("targetRayMode", Napi::Number::New(env, gazeInputSource->targetRayMode));
    jsInputSource.Set("handedness", Napi::Number::New(env, gazeInputSource->handness));
    return jsInputSource;
  }

  Napi::Value XRDeviceNative::GetHandInputSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
    ;
  }

  Napi::Value XRDeviceNative::GetGamepadInputSources(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
    ;
  }

  Napi::Value XRDeviceNative::GetScreenInputSources(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
    ;
  }

  Napi::Value XRDeviceNative::StartFrame(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::StartFrame: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "XRDeviceNative::StartFrame: expected 3 arguments")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::StartFrame: sessionId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::StartFrame: stereoRenderingId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::StartFrame: passId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // TODO: support stereoRenderingFrames by sessions.
    auto sessionId = info[0].As<Napi::Number>().Int32Value();
    auto stereoRenderingId = info[1].As<Napi::Number>().Int32Value();
    auto passId = info[2].As<Napi::Number>().Int32Value();

    if (device->startFrame(sessionId, stereoRenderingId, passId) == false)
    {
      Napi::TypeError::New(env, "XRDeviceNative::StartFrame: failed to start frame")
          .ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  Napi::Value XRDeviceNative::EndFrame(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::EndFrame: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "XRDeviceNative::EndFrame: expected 3 arguments")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::EndFrame: sessionId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::EndFrame: stereoRenderingId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::EndFrame: passId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // TODO: support stereoRenderingFrames by sessions.
    auto sessionId = info[0].As<Napi::Number>().Int32Value();
    auto stereoRenderingId = info[1].As<Napi::Number>().Int32Value();
    auto passId = info[2].As<Napi::Number>().Int32Value();

    if (device->endFrame(sessionId, stereoRenderingId, passId) == false)
    {
      Napi::TypeError::New(env, "XRDeviceNative::EndFrame: failed to end frame")
          .ThrowAsJavaScriptException();
    }
    return env.Undefined();
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

  bool XRDeviceNative::startFrame(uint32_t sessionId, uint32_t stereoRenderingId, uint32_t passIndex)
  {
    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      return false;
    }
    return device->startFrame(sessionId, stereoRenderingId, passIndex);
  }

  bool XRDeviceNative::endFrame(uint32_t sessionId, uint32_t stereoRenderingId, uint32_t passIndex)
  {
    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      return false;
    }
    return device->endFrame(sessionId, stereoRenderingId, passIndex);
  }

  TrViewport XRDeviceNative::getViewport(uint32_t viewIndex)
  {
    // TODO
    return TrViewport();
  }

  void XRDeviceNative::handleFrameRequest(xr::TrXRFrameRequest *frameRequest)
  {
    auto data = new XRFrameRequestContext{this, frameRequest};
    tsfnWithFrameHandler.NonBlockingCall(data, [](Napi::Env env, Napi::Function jsCallback, XRFrameRequestContext *context)
                                         { jsCallback.Call({Napi::External<XRFrameRequestContext>::New(env, context)}); });
  }
}
