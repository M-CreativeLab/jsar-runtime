#include <cmath>
#include <glm/glm.hpp>
#include <common/utility.hpp>

#include "./session.hpp"
#include "./common.hpp"
#include "./render_state.hpp"
#include "./frame.hpp"
#include "./space.hpp"
#include "./input_source.hpp"

namespace bindings
{
#define FRAME_TIME_DELTA_THRESHOLD 1000 / 45

  using namespace std;
  using namespace glm;
  using namespace Napi;

  thread_local FunctionReference *XRSession::constructor;
  thread_local uint32_t XRFrameCallbackDescriptor::NEXT_HANDLE = 0;

  // static
  void XRSession::Init(Napi::Env env)
  {
#define MODULE_NAME "XRSession"
    auto props = dombinding::EventTargetWrap<XRSession, client_xr::XRSession>::GetClassProperties();
    auto added = vector<ClassPropertyDescriptor<XRSession>>(
        {InstanceAccessor("inputSources", &XRSession::InputSourcesGetter, nullptr, napi_default_jsproperty),
         InstanceAccessor("renderState", &XRSession::RenderStateGetter, nullptr, napi_default_jsproperty),
         InstanceAccessor("environmentBlendMode", &XRSession::EnvironmentBlendModeGetter, nullptr, napi_default_jsproperty),
         InstanceMethod("requestAnimationFrame", &XRSession::RequestAnimationFrame, napi_default_method),
         InstanceMethod("cancelAnimationFrame", &XRSession::CancelAnimationFrame, napi_default_method),
         InstanceMethod("requestReferenceSpace", &XRSession::RequestReferenceSpace, napi_default_method),
         InstanceMethod("updateRenderState", &XRSession::UpdateRenderState, napi_default_method),
         InstanceMethod("updateTargetFrameRate", &XRSession::UpdateTargetFrameRate, napi_default_method),
         InstanceMethod("updateCollisionBox", &XRSession::UpdateCollisionBox, napi_default_method),
         InstanceMethod("end", &XRSession::End, napi_default_method)});
    props.insert(props.end(), added.begin(), added.end());

    Function tpl = DefineClass(env, MODULE_NAME, props);
    constructor = new FunctionReference();
    *constructor = Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set(MODULE_NAME, tpl);
#undef MODULE_NAME
  }

  XRSession::XRSession(const CallbackInfo &info)
      : XRHandleWrap<XRSession, client_xr::XRSession, dombinding::EventTargetWrap>(info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    // Update the `handle_` to event target
    setEventTarget(handle_);

    // Initialize the frame dispatcher
    frameDispatcherTsfn_ = ThreadSafeFunction::New(
        env,
        Function::New(env, [](const auto &info) {}),
        "XRSession::FrameDispatcher", 0, 2);

    auto dispatcher = [this](shared_ptr<xr::TrXRFrameRequest> frameRequest, client_xr::XRFrameDispatcherCallback dispatch)
    {
      frameDispatcherTsfn_.NonBlockingCall([frameRequest, dispatch](Napi::Env env, Function jsCallback)
                                           { dispatch(frameRequest, (napi_env)env); });
    };
    /**
     * Configure the WebXR session to use the provided frame dispatcher, which will dispatch the WebXR frame to the JavaScript
     * thread.
     */
    handle_->setXRFrameDispatcher(dispatcher);

    /**
     * Initialize the object references.
     */
    inputSourcesRef_ = Persistent(XRInputSourceArray::New(env, handle_->inputSources));
    enabledFeaturesRef_ = Persistent(createEnabledFeatures(env));

    // Define JS properties
    auto jsThis = info.This().ToObject();
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("recommendedContentSize",
                                                          Napi::Number::New(env, handle_->recommendedContentSize),
                                                          napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("enabledFeatures",
                                                          enabledFeaturesRef_.Value(),
                                                          napi_enumerable));

    Ref();              // Ref the WebXR session object to keep it alive until the `end()` method is called.
    handle_->ref(this); // Ref the handle object to allow this object to be accessed from the handle object.
  }

  XRSession::~XRSession()
  {
    handle_->unref();
    frameDispatcherTsfn_.Release();
  }

  Value XRSession::InputSourcesGetter(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);
    auto inputSources = inputSourcesRef_.Value();
    inputSources.update(handle_->inputSources);
    return inputSources;
  }

  Value XRSession::RenderStateGetter(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);
    return XRRenderState::NewInstance(env, handle_->renderState());
  }

  Value XRSession::EnvironmentBlendModeGetter(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    auto mode = handle_->environmentBlendMode();
    return String::New(env, client_xr::to_string(mode));
  }

  Value XRSession::RequestAnimationFrame(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    if (info.Length() < 1)
    {
      TypeError::New(env, "One argument is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsFunction())
    {
      TypeError::New(env, "Argument must be a function").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto frameCallbackRef = new FunctionReference();
    *frameCallbackRef = Persistent(info[0].As<Function>());

    auto frameHandler = [this, frameCallbackRef](uint32_t time,
                                                 std::shared_ptr<client_xr::XRFrame> frame,
                                                 void *envData)
    {
      Napi::Env env((napi_env)envData); // covert void* to napi_env
      Napi::HandleScope scope(env);

      frameCallbackRef->Call({Number::New(env, time),
                              XRFrame::NewInstance(env, this, frame)});
      frameCallbackRef->Reset();
      delete frameCallbackRef;
    };
    auto id = handle_->requestAnimationFrame(frameHandler);
    return Number::New(env, id);
  }

  Value XRSession::CancelAnimationFrame(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    if (info.Length() < 1)
    {
      TypeError::New(env, "One argument is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      TypeError::New(env, "Argument must be a number").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    handle_->cancelAnimationFrame(info[0].As<Number>().Uint32Value());
    return env.Undefined();
  }

  Value XRSession::RequestReferenceSpace(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);
    auto deferred = Promise::Deferred::New(env);

    if (info.Length() < 1)
    {
      deferred.Reject(String::New(env, "One argument is required"));
      return deferred.Promise();
    }

    auto typeString = info[0].ToString().Utf8Value();
    try
    {
      deferred.Resolve(XRReferenceSpace::NewInstance(env, handle_->requestReferenceSpace(typeString)));
      return deferred.Promise();
    }
    catch (const exception &e)
    {
      deferred.Reject(String::New(env, e.what()));
      return deferred.Promise();
    }
  }

  Value XRSession::UpdateRenderState(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    if (handle_->ended)
    {
      auto msg = "Failed to update state: session(" + std::to_string(id()) + ") has been ended";
      TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 1 || !info[0].IsObject())
    {
      TypeError::New(env, "Failed to update state: a state object is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto newStateObject = info[0].As<Napi::Object>();
    if (newStateObject.Has("baseLayer"))
    {
      auto baseLayer = XRWebGLLayer::Unwrap(newStateObject.Get("baseLayer").As<Napi::Object>());
      if (baseLayer->session == nullptr)
      {
        TypeError::New(env, "Invalid baseLayer").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      if (baseLayer->session->id() != id())
      {
        TypeError::New(env, "baseLayer is not associated with this session").ThrowAsJavaScriptException();
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
          TypeError::New(env, "Cannot set inlineVerticalFieldOfView for immersive sessions").ThrowAsJavaScriptException();
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

    try
    {
      handle_->updateRenderState(newState);
    }
    catch (const std::exception &e)
    {
      TypeError::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  Value XRSession::UpdateTargetFrameRate(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    if (info.Length() < 1)
    {
      TypeError::New(env, "One argument is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    handle_->updateTargetFrameRate(info[0].As<Napi::Number>().FloatValue());
    return env.Undefined();
  }

  Value XRSession::UpdateCollisionBox(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    if (info.Length() < 2)
    {
      TypeError::New(env, "updateCollisionBox requires 2 arguments: min and max").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    vec3 minVec, maxVec;
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

  Value XRSession::End(const CallbackInfo &info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    handle_->end();
    Unref();
    return env.Undefined();
  }

  Napi::Array XRSession::createEnabledFeatures(Napi::Env env)
  {
    auto enabledFeaturesArray = Napi::Array::New(env);
    uint32_t index = 0;
    for (auto feat : handle_->enabledFeatures)
      enabledFeaturesArray.Set(index++, Napi::String::New(env, to_string(feat)));
    return enabledFeaturesArray;
  }
}
