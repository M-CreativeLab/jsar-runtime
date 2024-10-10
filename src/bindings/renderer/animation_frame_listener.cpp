#include "animation_frame_listener.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *AnimationFrameListener::constructor;
  Napi::Object AnimationFrameListener::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "AnimationFrameListener",
                                      {InstanceMethod("connect", &AnimationFrameListener::Connect),
                                       InstanceMethod("isConnected", &AnimationFrameListener::IsConnected)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("AnimationFrameListener", func);
    return exports;
  }

  AnimationFrameListener::AnimationFrameListener(const Napi::CallbackInfo &info) : Napi::ObjectWrap<AnimationFrameListener>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    clientContext = TrClientContextPerProcess::Get();
    if (!clientContext)
    {
      Napi::Error::New(env, "Failed to get client context").ThrowAsJavaScriptException();
      return;
    }

    connected = false;
    napi_get_uv_event_loop(env, &eventloop);
    tickHandle.data = this;
    uv_timer_init(eventloop, &tickHandle);
  }

  AnimationFrameListener::~AnimationFrameListener()
  {
    if (!connected)
      return;
    uv_timer_stop(&tickHandle);
    onframeTsfn.Release();
  }

  Napi::Value AnimationFrameListener::Connect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (connected)
    {
      Napi::Error::New(env, "Already connected").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 1 || !info[0].IsFunction())
    {
      Napi::Error::New(env, "Invalid arguments, a function is required").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Function callback = info[0].As<Napi::Function>();
    onframeTsfn = Napi::ThreadSafeFunction::New(env, callback, "onframe", 0, 2);
    connected = true; // mark the `connected` to be true before `requestFrame()`.

    uv_timer_start(&tickHandle, [](uv_timer_t *handle)
                  {
      auto self = static_cast<AnimationFrameListener *>(handle->data);
      self->tick();
                  }, 0, 1);
    return env.Undefined();
  }

  Napi::Value AnimationFrameListener::IsConnected(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, connected);
  }

#define FRAME_TIME_DELTA_THRESHOLD 1000 / 45
  void AnimationFrameListener::tick()
  {
    if (TR_UNLIKELY(!connected))
      return;

    static chrono::steady_clock::time_point lastFrameTime = chrono::steady_clock::now();
    auto frameTime = chrono::steady_clock::now();
    auto delta = chrono::duration_cast<chrono::milliseconds>(frameTime - lastFrameTime).count();
    if (delta >= FRAME_TIME_DELTA_THRESHOLD)
    {
      lastFrameTime = frameTime;
      onFrameRequest();
    }
  }

  void AnimationFrameListener::onFrameRequest()
  {
    TrAnimationFrameRequest *frameRequest = new TrAnimationFrameRequest();
    frameRequest->resetTime();
    onframeTsfn.NonBlockingCall(frameRequest, [](Napi::Env env, Napi::Function jsCallback, TrAnimationFrameRequest *request)
                                {
      auto timeValue = Napi::Number::New(env, request->time);
      delete request;
      jsCallback.Call({timeValue}); });
  }
}
