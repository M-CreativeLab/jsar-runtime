#include <cmath>
#include <common/utility.hpp>
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

  Napi::Object XRSession::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRSession> handle)
  {
    Napi::EscapableHandleScope scope(env);
    SharedReference<client_xr::XRSession> handleRef(handle);
    auto handleExternal = Napi::External<SharedReference<client_xr::XRSession>>::New(env, &handleRef);
    Napi::Object instance = constructor->New({handleExternal});
    return scope.Escape(instance).ToObject();
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
    return env.Undefined();
  }

  XRSession::XRSession(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<XRSession>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() <= 0 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor")
          .ThrowAsJavaScriptException();
      return;
    }

    auto handleExternal = info[0].As<Napi::External<SharedReference<client_xr::XRSession>>>();
    auto handleRef = handleExternal.Data();
    if (handleRef == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor")
          .ThrowAsJavaScriptException();
      return;
    }
    handle_ = handleRef->value;
    assert(handle_ != nullptr);

    // Set the event callback
    // onEventCallback = Napi::Persistent(info[3].As<Napi::Function>());

    // Define JS properties
    auto jsThis = info.This().ToObject();
    // jsThis.DefineProperty(Napi::PropertyDescriptor::Value("recommendedContentSize", nativeSessionObject.Get("recommendedContentSize"), napi_enumerable));
    // jsThis.DefineProperty(Napi::PropertyDescriptor::Value("inputSources", inputSources.Value(), napi_enumerable));
    // jsThis.DefineProperty(Napi::PropertyDescriptor::Value("enabledFeatures", enabledFeatures.Value(), napi_enumerable));

    // // Prepare the frame handler
    // frameHandlerRef = new Napi::FunctionReference();
    // *frameHandlerRef = Napi::Persistent(Napi::Function::New(env, FrameHandler));
    // frameHandlerTSFN = Napi::ThreadSafeFunction::New(env, frameHandlerRef->Value(), "XRSession::FrameHandler", 0, 2);

    // // Prepare the uv handles
    // napi_get_uv_event_loop(env, &eventloop);
    // tickHandle.data = this;
    // uv_timer_init(eventloop, &tickHandle);

    // // Start the session
    // start();

    // if (started == true)
    //   Ref(); // If started, we need increment the reference count to prevent the object from being garbage collected.
  }

  Napi::Value XRSession::RenderStateGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return XRRenderState::NewInstance(env, handle_->renderState());
  }

  Napi::Value XRSession::EnvironmentBlendModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto mode = handle_->environmentBlendMode();
    return Napi::String::New(env, client_xr::to_string(mode));
  }

  Napi::Value XRSession::RequestAnimationFrame(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

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

    Napi::ThreadSafeFunction *tscb = nullptr;
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
        env,
        info[0].As<Napi::Function>(),
        "XRSession::RequestAnimationFrame",
        0,
        2,
        [tscb](Napi::Env env)
        {
          if (tscb != nullptr)
            delete tscb;
        });
    tscb = new Napi::ThreadSafeFunction(tsfn);
    tscb->Acquire();
    std::cout << "RequestAnimationFrame Set callback done" << std::endl;

    auto callback = [tscb](uint32_t time, client_xr::XRFrame &frame)
    {
      std::cout << "RequestAnimationFrame callback called" << std::endl;

      assert(tscb != nullptr);
      tscb->BlockingCall([time](Napi::Env env, Napi::Function jsCallback)
                         { jsCallback.Call({Napi::Number::New(env, time)}); });
      tscb->Release();
    };
    auto id = handle_->requestAnimationFrame(callback);
    return Napi::Number::New(env, id);
  }

  Napi::Value
  XRSession::CancelAnimationFrame(const Napi::CallbackInfo &info)
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

    handle_->cancelAnimationFrame(info[0].As<Napi::Number>().Uint32Value());
    return env.Undefined();
  }

  Napi::Value XRSession::RequestReferenceSpace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    auto deferred = Napi::Promise::Deferred::New(env);

    if (info.Length() < 1)
    {
      deferred.Reject(Napi::String::New(env, "One argument is required"));
      return deferred.Promise();
    }

    auto typeString = info[0].ToString().Utf8Value();
    try
    {
      deferred.Resolve(XRReferenceSpace::NewInstance(env, handle_->requestReferenceSpace(typeString)));
      return deferred.Promise();
    }
    catch(const std::exception& e)
    {
      deferred.Reject(Napi::String::New(env, e.what()));
      return deferred.Promise();
    }
  }

  Napi::Value XRSession::UpdateRenderState(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (handle_->ended)
    {
      auto msg = "Failed to update state: session(" + std::to_string(id()) + ") has been ended";
      Napi::Error::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 1 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "Failed to update state: a state object is required").ThrowAsJavaScriptException();
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
      if (baseLayer->session->id() != id())
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
        if (immersive() == true)
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

    client_xr::XRRenderState newState;
    if (newStateObject.Has("baseLayer"))
    {
      auto baseLayer = XRWebGLLayer::Unwrap(newStateObject.Get("baseLayer").ToObject());
      newState.baseLayer = baseLayer->handle();
    }
    if (newStateObject.Has("depthNear"))
      newState.depthNear = newStateObject.Get("depthNear").ToNumber().FloatValue();
    if (newStateObject.Has("depthFar"))
      newState.depthFar = newStateObject.Get("depthFar").ToNumber().FloatValue();
    if (newStateObject.Has("inlineVerticalFieldOfView"))
      newState.inlineVerticalFieldOfView = newStateObject.Get("inlineVerticalFieldOfView").ToNumber().FloatValue();

    handle_->updateRenderState(newState);
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

    handle_->updateTargetFrameRate(info[0].As<Napi::Number>().FloatValue());
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

    glm::vec3 minVec, maxVec;
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

      minVec[i] = minValue;
      maxVec[i] = maxValue;
    }

    handle_->updateCollisionBox(minVec, maxVec);
    return env.Undefined();
  }

  Napi::Value XRSession::End(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    handle_->end();
    Unref();
    return env.Undefined();
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
}
