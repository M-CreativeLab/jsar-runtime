#include "animation_frame_listener.hpp"

namespace bindings
{
  Napi::FunctionReference *AnimationFrameListener::constructor;
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
  }

  AnimationFrameListener::~AnimationFrameListener()
  {
    if (!connected)
      return;
    if (internalRequestId > 0)
      clientContext->cancelFrame(internalRequestId);
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
    onframeTsfn = Napi::ThreadSafeFunction::New(env, callback, "onframe", 0, 1);
    connected = true; // mark the `connected` to be true before `requestFrame()`.
    internalRequestId = clientContext->requestFrame([this](AnimationFrameRequest &request)
                                                    { onFrameRequest(request); });
    return env.Undefined();
  }

  Napi::Value AnimationFrameListener::IsConnected(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, connected);
  }

  void AnimationFrameListener::onFrameRequest(AnimationFrameRequest &request)
  {
    if (!connected)
      return;
    onframeTsfn.BlockingCall(&request, [](Napi::Env env, Napi::Function jsCallback, AnimationFrameRequest *request)
                             {
      auto timeValue = Napi::Number::New(env, request->time);
      jsCallback.Call({timeValue}); });
  }
}
