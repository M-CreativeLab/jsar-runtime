#include <cmath>
#include "session.hpp"
#include "common.hpp"
#include "render_state.hpp"
#include "frame.hpp"
#include "space.hpp"
#include "input_source.hpp"

namespace bindings
{
#define FRAME_TIME_DELTA_THRESHOLD 1000 / 45

  thread_local Napi::FunctionReference *XRSession::constructor;
  thread_local uint32_t XRFrameCallbackDescriptor::NEXT_HANDLE = 0;

  Napi::Object XRSession::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRSession",
                                     {InstanceAccessor("renderState", &XRSession::RenderStateGetter, nullptr),
                                      InstanceAccessor("environmentBlendMode", &XRSession::EnvironmentBlendModeGetter, nullptr),
                                      InstanceMethod("requestAnimationFrame", &XRSession::RequestAnimationFrame),
                                      InstanceMethod("cancelAnimationFrame", &XRSession::CancelAnimationFrame),
                                      InstanceMethod("requestReferenceSpace", &XRSession::RequestReferenceSpace),
                                      InstanceMethod("updateRenderState", &XRSession::UpdateRenderState),
                                      InstanceMethod("updateTargetFrameRate", &XRSession::UpdateTargetFrameRate),
                                      InstanceMethod("updateCollisionBox", &XRSession::UpdateCollisionBox),
                                      InstanceMethod("end", &XRSession::End)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRSession", tpl);
    return exports;
  }

  inline xr::TrXRFrameRequest createFrameRequestForView(uint32_t viewIndex, XRFrameContext &frameContext)
  {
    auto req = xr::TrXRFrameRequest();
    req.sessionId = frameContext.sessionId;
    req.stereoId = frameContext.stereoId;
    req.stereoTimestamp = frameContext.stereoTimestamp;
    memcpy(req.localBaseMatrix, frameContext.localBaseMatrix, sizeof(float) * 16);
    memcpy(req.viewerBaseMatrix, frameContext.viewerBaseMatrix, sizeof(float) * 16);
    req.views[0] = frameContext.views[0];
    req.views[1] = frameContext.views[1];
    req.viewIndex = viewIndex;
    return req;
  }

  Napi::Value XRSession::FrameHandler(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "expected an external.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto xrSession = XRSession::Unwrap(info[0].ToObject());
    if (TR_UNLIKELY(xrSession == nullptr || xrSession->ended))
    {
      fprintf(stderr, "skipped XRFrame(), reason is: session is ended\n");
      return env.Undefined();
    }
    if (TR_UNLIKELY(xrSession->id < 0))
    {
      fprintf(stderr, "skipped XRFrameRequest(), reason is: session is invalid\n");
      return env.Undefined();
    }

    static int prevStereoId = -1;
    auto sessionContext = xrSession->sessionContextZoneClient->getData();
    auto deviceContext = xrSession->device->clientContext->getXRDeviceContextZone()->getData();
    if (prevStereoId != -1 && prevStereoId == sessionContext->stereoId)
      return env.Undefined();

    int pendingsAtServer = sessionContext->getPendingStereoFramesCount();
    if (pendingsAtServer >= 2)
      return env.Undefined(); // Skip the frame if there are more than 2 pending frames.
    if (!sessionContext->inFrustum)
      return env.Undefined(); // Skip the frame if the session is not in the frustum.

    XRFrameContext frameContext(*sessionContext, *deviceContext, xrSession);
    /**
     * TODO: support singlepass rendering
     */
    for (uint32_t viewIndex = 0; viewIndex < 2; viewIndex++)
    {
      auto req = createFrameRequestForView(viewIndex, frameContext);
      xrSession->onFrame(env, &req);
    }
    prevStereoId = sessionContext->stereoId;
    return env.Undefined();
  }

  XRSession::XRSession(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRSession>(info),
                                                         id(-1),
                                                         mode(xr::TrXRSessionMode::Unknown),
                                                         immersive(true),
                                                         started(false),
                                                         ended(false),
                                                         suspended(false),
                                                         device(nullptr)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "Three arguments are required").ThrowAsJavaScriptException();
      return;
    }
    if (!info[2].IsObject())
    {
      Napi::TypeError::New(env, "The 3rd argument must be an object").ThrowAsJavaScriptException();
      return;
    }
    if (!info[3].IsFunction())
    {
      Napi::TypeError::New(env, "The 4th argument must be a function").ThrowAsJavaScriptException();
      return;
    }

    auto deviceObject = info[0].As<Napi::Object>();
    device = XRDeviceNative::Unwrap(deviceObject);

    auto modeString = info[1].As<Napi::String>().Utf8Value();
    mode = xr::MakeSessionMode(modeString);
    if (mode == xr::TrXRSessionMode::Unknown)
    {
      Napi::TypeError::New(env, "Invalid session mode").ThrowAsJavaScriptException();
      return;
    }

    auto nativeSessionObject = info[2].ToObject();
    id = nativeSessionObject.Get("id").ToNumber().Int32Value();
    immersive = xr::IsImmersive(mode);

    // Create zone client
    auto clientContext = TrClientContextPerProcess::Get();
    string zonePath = clientContext->xrDeviceInit.sessionContextZoneDirectory + "/" + to_string(id);
    sessionContextZoneClient = make_unique<xr::TrXRSessionContextZone>(zonePath, TrZoneType::Client);

    // Create the view spaces
    if (immersive)
    {
      addViewSpace(env, XRViewSpaceType::LEFT);
      addViewSpace(env, XRViewSpaceType::RIGHT);
    }
    else
    {
      addViewSpace(env, XRViewSpaceType::NONE);
    }

    // Create the initial `XRRenderState` object
    activeRenderState = new xr::RenderState();
    pendingRenderState = nullptr;
    enabledFeatures = Napi::Persistent(createEnabledFeatures(env));

    // Create ReferenceSpace instances.
    localSpace = Napi::Persistent(XRReferenceSpace::NewInstance(env, XRReferenceSpaceType::LOCAL));
    viewerSpace = Napi::Persistent(XRReferenceSpace::NewInstance(env, XRReferenceSpaceType::VIEWER));
    unboundedSpace = Napi::Persistent(XRReferenceSpace::NewInstance(env, XRReferenceSpaceType::UNBOUNDED));

    // Create the `XRInputSourceArray` object
    inputSources = Napi::Persistent(XRInputSourceArray::New(env));

    // Set the event callback
    onEventCallback = Napi::Persistent(info[3].As<Napi::Function>());

    // Define JS properties
    auto jsThis = info.This().ToObject();
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("recommendedContentSize", nativeSessionObject.Get("recommendedContentSize"), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("inputSources", inputSources.Value(), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("enabledFeatures", enabledFeatures.Value(), napi_enumerable));

    // Prepare the frame handler
    frameHandlerRef = new Napi::FunctionReference();
    *frameHandlerRef = Napi::Persistent(Napi::Function::New(env, FrameHandler));
    frameHandlerTSFN = Napi::ThreadSafeFunction::New(env, frameHandlerRef->Value(), "XRSession::FrameHandler", 0, 2);

    // Prepare the uv handles
    napi_get_uv_event_loop(env, &eventloop);
    tickHandle.data = this;
    uv_timer_init(eventloop, &tickHandle);

    // Start the session
    start();

    if (started == true)
      Ref(); // If started, we need increment the reference count to prevent the object from being garbage collected.
  }

  XRSession::~XRSession()
  {
    DEBUG(LOG_TAG, "XRSession(%d) is being deleted", id);

    if (activeRenderState != nullptr)
    {
      delete activeRenderState;
      activeRenderState = nullptr;
    }
    if (pendingRenderState != nullptr)
    {
      delete pendingRenderState;
      pendingRenderState = nullptr;
    }
    for (auto &it : pendingFrameCallbacks)
      delete it;
    pendingFrameCallbacks.clear();

    for (auto &it : currentFrameCallbacks)
      delete it;
    currentFrameCallbacks.clear();

    // if (device != nullptr)
    //   device->removeSession(id);

    localSpace.Unref();
    viewerSpace.Unref();

    // Clear the view spaces
    for (auto &it : viewSpaces)
    {
      it->Unref();
      delete it;
    }
    viewSpaces.clear();
  }

  Napi::Value XRSession::RenderStateGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return XRRenderState::NewInstance(env, *activeRenderState);
  }

  Napi::Value XRSession::EnvironmentBlendModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (environmentBlendMode == OPAQUE)
    {
      return Napi::String::New(env, "opaque");
    }
    else if (environmentBlendMode == ADDITIVE)
    {
      return Napi::String::New(env, "additive");
    }
    else if (environmentBlendMode == ALPHA_BLEND)
    {
      return Napi::String::New(env, "alpha-blend");
    }
    else
    {
      return Napi::String::New(env, "unknown");
    }
  }

  Napi::Value XRSession::RequestAnimationFrame(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (ended == true)
    {
      Napi::Error::New(env, "Session has ended").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "One argument is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsFunction())
    {
      Napi::TypeError::New(env, "Argument must be a function").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto callback = info[0].As<Napi::Function>();
    auto frameCallbackDescriptor = new XRFrameCallbackDescriptor(callback);
    pendingFrameCallbacks.push_back(frameCallbackDescriptor);
    return Napi::Number::New(env, frameCallbackDescriptor->handle);
  }

  Napi::Value XRSession::CancelAnimationFrame(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "One argument is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Argument must be a number").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto targetHandle = info[0].As<Napi::Number>().Uint32Value();

    // Find the target callback handle from the pending list, then remove it directly.
    for (auto it = pendingFrameCallbacks.begin(); it != pendingFrameCallbacks.end();)
    {
      auto callbackDescriptor = *it;
      if (callbackDescriptor->handle == targetHandle)
      {
        delete callbackDescriptor;
        it = pendingFrameCallbacks.erase(it);
        break;
      }
      else
      {
        ++it;
      }
    }

    // Find the target callback handle from the current list, then mark it as cancelled.
    for (auto it = currentFrameCallbacks.begin(); it != currentFrameCallbacks.end();)
    {
      auto callbackDescriptor = *it;
      if (callbackDescriptor->handle == targetHandle)
      {
        callbackDescriptor->cancelled = true;
        break;
      }
      else
      {
        ++it;
      }
    }
    return env.Undefined();
  }

  Napi::Value XRSession::RequestReferenceSpace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    auto deferred = Napi::Promise::Deferred::New(env);

    if (ended == true)
    {
      deferred.Reject(Napi::String::New(env, "Session has ended"));
      return deferred.Promise();
    }

    if (info.Length() < 1)
    {
      deferred.Reject(Napi::String::New(env, "One argument is required"));
      return deferred.Promise();
    }

    auto typeString = info[0].As<Napi::String>().Utf8Value();
    XRReferenceSpaceType type;

    if (typeString == "viewer")
      type = VIEWER;
    else if (typeString == "local")
      type = LOCAL;
    else if (typeString == "local-floor")
      type = LOCAL_FLOOR;
    else if (typeString == "bounded-floor")
      type = BOUNDED_FLOOR;
    else if (typeString == "unbounded")
      type = UNBOUNDED;
    else
    {
      deferred.Reject(Napi::String::New(env, "Invalid reference space type"));
      return deferred.Promise();
    }

    if (!device->supportsReferenceSpaceType(type))
    {
      deferred.Reject(Napi::String::New(env, "Reference space type is not supported"));
      return deferred.Promise();
    }

    if (type == VIEWER)
    {
      deferred.Resolve(viewerSpace.Value());
    }
    else if (type == LOCAL)
    {
      deferred.Resolve(localSpace.Value());
    }
    else if (type == UNBOUNDED)
    {
      deferred.Resolve(unboundedSpace.Value());
    }
    else
    {
      deferred.Reject(Napi::String::New(env, "Not implemented yet"));
      // TODO: others
    }
    return deferred.Promise();
  }

  Napi::Value XRSession::UpdateRenderState(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (ended == true)
    {
      Napi::Error::New(env, "Session has ended").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "One argument is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject())
    {
      Napi::TypeError::New(env, "Argument must be an object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto newStateObject = info[0].As<Napi::Object>();
    if (newStateObject.Has("baseLayer"))
    {
      auto baseLayer = XRWebGLLayer::Unwrap(newStateObject.Get("baseLayer").As<Napi::Object>());
      if (baseLayer->session == nullptr)
      {
        Napi::TypeError::New(env, "Invalid baseLayer").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      if (baseLayer->session->id != id)
      {
        Napi::TypeError::New(env, "baseLayer is not associated with this session").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    if (newStateObject.Has("inlineVerticalFieldOfView"))
    {
      auto inlineVerticalFov = newStateObject.Get("inlineVerticalFieldOfView");
      auto isFovSet = !inlineVerticalFov.IsNull() && !inlineVerticalFov.IsUndefined();
      if (isFovSet)
      {
        if (immersive == true)
        {
          Napi::TypeError::New(env, "Cannot set inlineVerticalFieldOfView for immersive sessions").ThrowAsJavaScriptException();
          return env.Undefined();
        }
        else
        {
          auto inlineVerticalFovValue = inlineVerticalFov.As<Napi::Number>().FloatValue();
          auto newFovValue = fmin(3.13, fmax(0.01, inlineVerticalFovValue));
          newStateObject.Set("inlineVerticalFieldOfView", Napi::Number::New(env, newFovValue));
        }
      }
    }

    if (pendingRenderState == nullptr)
    {
      if (activeRenderState != nullptr)
        pendingRenderState = new xr::RenderState(activeRenderState);
      else
        pendingRenderState = new xr::RenderState();
    }

    if (newStateObject.Has("baseLayer"))
    {
      auto baseLayer = XRWebGLLayer::Unwrap(newStateObject.Get("baseLayer").ToObject());
      pendingRenderState->updateBaseLayer(&baseLayer->config);
    }
    if (newStateObject.Has("depthNear"))
      pendingRenderState->depthNear = newStateObject.Get("depthNear").ToNumber().FloatValue();
    if (newStateObject.Has("depthFar"))
      pendingRenderState->depthFar = newStateObject.Get("depthFar").ToNumber().FloatValue();
    if (newStateObject.Has("inlineVerticalFieldOfView"))
      pendingRenderState->inlineVerticalFieldOfView = newStateObject.Get("inlineVerticalFieldOfView").ToNumber().FloatValue();
    return env.Undefined();
  }

  Napi::Value XRSession::UpdateTargetFrameRate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "One argument is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // auto frameRate = info[0].As<Napi::Number>().FloatValue();
    // device->updateTargetFrameRate(frameRate);
    return env.Undefined();
  }

  Napi::Value XRSession::UpdateCollisionBox(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "updateCollisionBox requires 2 arguments: min and max").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float minValues[3];
    float maxValues[3];
    auto min = info[0].As<Napi::Array>();
    auto max = info[1].As<Napi::Array>();

    for (int i = 0; i < 3; i++)
    {
      auto minValue = min.Get(i).ToNumber().FloatValue();
      auto maxValue = max.Get(i).ToNumber().FloatValue();
      if (minValue > maxValue ||
          minValue == INFINITY ||
          maxValue == -INFINITY)
        return env.Undefined();

      minValues[i] = minValue;
      maxValues[i] = maxValue;
    }

    if (sessionContextZoneClient == nullptr)
    {
      Napi::Error::New(env, "Invalid XRSession object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    sessionContextZoneClient->setCollisionBoxMinMax(minValues, maxValues);
    return env.Undefined();
  }

  Napi::Value XRSession::End(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    stop();
    Unref();
    return env.Undefined();
  }

  void XRSession::start()
  {
    if (started == true)
      return; // Already started

    uv_timer_start(&tickHandle, [](uv_timer_t *handle)
                   {
                    auto session = static_cast<XRSession *>(handle->data);
                    session->tick(); }, 0, 2);
    started = true;
  }

  void XRSession::stop()
  {
    if (started == false)
      return; // Not started yet

    uv_timer_stop(&tickHandle);
    ended = true;
  }

  void XRSession::tick()
  {
    static chrono::steady_clock::time_point timepointOnLastTick = chrono::steady_clock::now();
    chrono::steady_clock::time_point timepointOnNow = chrono::steady_clock::now();
    auto delta = chrono::duration_cast<chrono::milliseconds>(timepointOnNow - timepointOnLastTick).count();
    if (delta >= FRAME_TIME_DELTA_THRESHOLD)
    {
      frameHandlerTSFN.NonBlockingCall(this, [](Napi::Env env, Napi::Function jsCallback, XRSession *session)
                                       { jsCallback.Call({session->Value()}); });
      timepointOnLastTick = timepointOnNow;
    }
  }

  bool XRSession::calcFps()
  {
    frameCount += 1;
    auto delta = chrono::duration_cast<chrono::milliseconds>(frameTimepoint - lastRecordedFrameTimepoint).count();
    if (delta >= 1000)
    {
      fps = frameCount / (delta / 1000);
      frameCount = 0;
      lastRecordedFrameTimepoint = frameTimepoint;
      return true;
    }
    else
    {
      return false;
    }
  }

  void XRSession::updateFrameTime(bool updateStereoFrame)
  {
    frameTimepoint = chrono::steady_clock::now();
    if (updateStereoFrame)
      lastStereoFrameTimepoint = frameTimepoint;
  }

  void XRSession::updateInputSourcesIfChanged(XRFrame *frame)
  {
    inputSources.Value().updateInputSources(frame, this, [this](vector<XRInputSource *> added, vector<XRInputSource *> removed)
                                            { onEventCallback.Call({Napi::String::New(Env(), "inputsourceschange"),
                                                                    createInputSourcesChangeEvent(Env(), added, removed)}); });
  }

  void XRSession::onFrame(Napi::Env env, xr::TrXRFrameRequest *frameRequest)
  {
    Napi::HandleScope scope(env);

    // - If session’s pending render state is not null, apply the pending render state.
    if (pendingRenderState != nullptr)
    {
      // Apply pending render state.
      activeRenderState->update(pendingRenderState);

      // Clear the pending render state.
      delete pendingRenderState;
      pendingRenderState = nullptr;

      // Report to the device since it'll need to handle the layer for rendering.
      if (activeRenderState->baseLayer != nullptr)
      {
        // device->setActiveLayer(activeRenderState->baseLayer);
      }
    }

    // - If session’s renderState's baseLayer is null, abort these steps.
    if (activeRenderState == nullptr || activeRenderState->baseLayer == nullptr)
    {
      std::cerr << "activeRenderState or activeRenderState->baseLayer is null, aborting frame rendering." << std::endl;
      return;
    }

    // - If session’s mode is "inline" and session’s renderState's output canvas is null,
    //   abort these steps.
    // ???

    auto xrFrameObject = XRFrame::NewInstance(env, frameRequest, this);
    auto xrFrameUnwrapped = XRFrame::Unwrap(xrFrameObject);

    // Move the pending frame callbacks to current map
    currentFrameCallbacks.clear();
    for (auto &it : pendingFrameCallbacks)
      currentFrameCallbacks.push_back(it);
    pendingFrameCallbacks.clear();

    xrFrameUnwrapped->start();
    // Update the input sources
    updateInputSourcesIfChanged(xrFrameUnwrapped);

    // Call all the frame callbacks
    auto time = Napi::Number::New(env, frameRequest->time);
    for (auto &it : currentFrameCallbacks)
    {
      auto descriptor = *it;
      if (descriptor.cancelled != true)
      {
        auto callback = descriptor.callback;
        try
        {
          descriptor.callback->Call(this->Value(), {time, xrFrameObject});
        }
        catch (const Napi::Error &err)
        {
          string errorDescription = "Unknown frame error.";
          if (!err.IsEmpty())
          {
            auto stack = err.Get("stack");
            auto message = err.Get("message");
            if (stack.IsString())
              errorDescription = stack.As<Napi::String>().Utf8Value();
            else if (message.IsString())
              errorDescription = message.As<Napi::String>().Utf8Value();
          }
          std::cerr << "An error occurred in frame: " << errorDescription << std::endl;
        }
      }
    }
    currentFrameCallbacks.clear();
    xrFrameUnwrapped->end();
  }

  void XRSession::addViewSpace(Napi::Env env, XRViewSpaceType type)
  {
    auto xrViewSpaceRef = new Napi::ObjectReference();
    *xrViewSpaceRef = Napi::Persistent(XRViewSpace::NewInstance(env, type));
    viewSpaces.push_back(xrViewSpaceRef);
  }

  Napi::Array XRSession::createEnabledFeatures(Napi::Env env)
  {
    auto enabledFeaturesArray = Napi::Array::New(env);
    enabledFeaturesArray.Set(uint32_t(0), Napi::String::New(env, "viewer"));
    enabledFeaturesArray.Set(uint32_t(1), Napi::String::New(env, "local"));
    return enabledFeaturesArray;
  }

  Napi::Value XRSession::createInputSourcesChangeEvent(Napi::Env env,
                                                       std::vector<XRInputSource *> &added,
                                                       std::vector<XRInputSource *> &removed)
  {
    auto eventObject = Napi::Object::New(env);
    auto addedArray = Napi::Array::New(env, added.size());
    auto removedArray = Napi::Array::New(env, removed.size());

    for (size_t i = 0; i < added.size(); i++)
    {
      auto inputSource = added[i];
      addedArray[i] = inputSource->Value();
    }
    for (size_t i = 0; i < removed.size(); i++)
    {
      auto inputSource = removed[i];
      removedArray[i] = inputSource->Value();
    }

    eventObject.Set("added", addedArray);
    eventObject.Set("removed", removedArray);
    return eventObject;
  }

  void XRSession::onPrimaryActionStart(XRInputSource *inputSource, XRFrame *frame)
  {
    Napi::Env env = Env();
    Napi::HandleScope scope(env);

    auto eventProps = Napi::Object::New(env);
    eventProps.Set("frame", frame->Value());
    eventProps.Set("inputSource", inputSource->Value());
    onEventCallback.Call({Napi::String::New(env, "selectstart"), eventProps});
  }

  void XRSession::onPrimaryActionEnd(XRInputSource *inputSource, XRFrame *frame)
  {
    Napi::Env env = Env();
    Napi::HandleScope scope(env);

    auto eventProps = Napi::Object::New(env);
    eventProps.Set("frame", frame->Value());
    eventProps.Set("inputSource", inputSource->Value());
    onEventCallback.Call({Napi::String::New(env, "select"), eventProps});
    onEventCallback.Call({Napi::String::New(env, "selectend"), eventProps});
  }

  void XRSession::onSqueezeActionStart(XRInputSource *inputSource, XRFrame *frame)
  {
    Napi::Env env = Env();
    Napi::HandleScope scope(env);

    auto eventProps = Napi::Object::New(env);
    eventProps.Set("frame", frame->Value());
    eventProps.Set("inputSource", inputSource->Value());
    onEventCallback.Call({Napi::String::New(env, "squeezestart"), eventProps});
  }

  void XRSession::onSqueezeActionEnd(XRInputSource *inputSource, XRFrame *frame)
  {
    Napi::Env env = Env();
    Napi::HandleScope scope(env);

    auto eventProps = Napi::Object::New(env);
    eventProps.Set("frame", frame->Value());
    eventProps.Set("inputSource", inputSource->Value());
    onEventCallback.Call({Napi::String::New(env, "squeeze"), eventProps});
    onEventCallback.Call({Napi::String::New(env, "squeezeend"), eventProps});
  }

  XRReferenceSpace *XRSession::getLocalSpace()
  {
    auto object = localSpace.Value();
    return XRReferenceSpace::Unwrap(object);
  }

  XRReferenceSpace *XRSession::getViewerSpace()
  {
    auto object = viewerSpace.Value();
    return XRReferenceSpace::Unwrap(object);
  }

  void XRSession::iterateViewSpaces(std::function<void(XRViewSpace *, uint32_t, XRSession *)> callback)
  {
    uint32_t viewIndex = 0;
    for (auto &it : viewSpaces)
    {
      auto object = (*it).Value();
      auto space = XRViewSpace::Unwrap(object);
      callback(space, viewIndex++, this);
    }
  }
}
