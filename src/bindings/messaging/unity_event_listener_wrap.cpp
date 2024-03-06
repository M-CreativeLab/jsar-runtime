#include "unity_event_listener_wrap.hpp"
#include "debug.hpp"

using namespace std;
using namespace bindings::messaging;

UnityEventListenerWrap *UnityEventListenerWrap::instance_;
Napi::FunctionReference *UnityEventListenerWrap::constructor;

void UnityEventListenerWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env,
      "NativeEventTarget",
      {InstanceMethod("setNativeEventListener", &UnityEventListenerWrap::SetNativeEventListener),
       InstanceMethod("dispatchEvent", &UnityEventListenerWrap::DispatchEvent),
       InstanceMethod("dispose", &UnityEventListenerWrap::Dispose)});

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  env.SetInstanceData(constructor);

  exports.Set("NativeEventTarget", tpl);
}

UnityEventListenerWrap *UnityEventListenerWrap::GetInstance()
{
  if (instance_ == nullptr)
    return nullptr;
  else
    return instance_;
}

UnityEventListenerWrap::UnityEventListenerWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<UnityEventListenerWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  instance_ = this;
  disposed_ = false;
  dispatch_available_ = false;
}

bool UnityEventListenerWrap::DispatchNativeEvent(int id, int type, string data)
{
  /**
   * Skip the dispatching if the instance is disposed or the dispatching is not available.
   */
  if (disposed_ || !dispatch_available_)
    return true;

  JSEventMessage *message = new JSEventMessage();
  message->id = id;
  message->type = type;
  message->data = data;

  auto callback = [](Napi::Env env, Napi::Function jsCallback, JSEventMessage *message)
  {
    try
    {
      Napi::Number idParam = Napi::Number::New(env, message->id);
      Napi::Number typeParam = Napi::Number::New(env, message->type);
      Napi::String dataParam = Napi::String::New(env, message->data);
      jsCallback.Call({idParam, typeParam, dataParam});
    }
    catch (const Napi::Error &e)
    {
      DEBUG("transmute", "NativeEventTarget::Dispatch() failed to call JS callback: %s", e.Message().c_str());
    }
    catch (...)
    {
      DEBUG("transmute", "NativeEventTarget::Dispatch() failed to call JS callback: Unkown exception.");
    }
    delete message;
  };

  napi_status status = tsfn_.NonBlockingCall(message, callback);
  if (status != napi_ok)
  {
    DEBUG("transmute", "NativeEventTarget::Dispatch() failed to call JS callback");
    return false;
  }
  else
  {
    return true;
  }
}

bool UnityEventListenerWrap::GetEvent(int *id, int *type, const char *data, uint32_t *size, bool popQueue = false)
{
  std::lock_guard<std::mutex> lock(events_queue_mutex_);
  if (events_queue_.empty())
    return false;

  auto call = events_queue_.front();
  if (id != nullptr)
    *id = call.id;
  if (type != nullptr)
    *type = call.type;
  if (size != nullptr)
    *size = call.data.length();
  if (data != nullptr)
  {
    auto src = call.data.c_str();
    auto len = call.data.length();
    memcpy((void *)data, src, len);
  }

  if (popQueue)
    events_queue_.pop();
  return true;
}

void UnityEventListenerWrap::Finalize(Napi::Env env)
{
  if (disposed_)
    return;

  tsfn_.Unref(env);
  tsfn_.Release();
  instance_ = nullptr;
  disposed_ = true;
  dispatch_available_ = false;
}

Napi::Value UnityEventListenerWrap::SetNativeEventListener(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (disposed_)
  {
    Napi::TypeError::New(env, "NativeEventTarget is already disposed").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (!info[0].IsFunction())
  {
    Napi::TypeError::New(env, "NativeEventTarget.setNativeEventListener() requires a function")
      .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  tsfn_ = Napi::ThreadSafeFunction::New(
      env,
      info[0].As<Napi::Function>(),
      "NativeEventTarget",
      0,
      1,
      [](Napi::Env env)
      {
        DEBUG("transmute", "NativeEventTarget listener is finalized.");
      });
  dispatch_available_ = true;
  DEBUG("transmute", "NativeEventTarget::SetNativeEventListener() is called.");
  return info.This();
}

Napi::Value UnityEventListenerWrap::DispatchEvent(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (disposed_)
  {
    Napi::TypeError::New(env, "UnityEventListener is already disposed")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (info.Length() < 3)
  {
    Napi::TypeError::New(env, "MakeCall(id, type, data) requires 3 arguments")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (!info[0].IsNumber())
  {
    Napi::TypeError::New(env, "MakeCall() requires the first argument to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (!info[1].IsNumber())
  {
    Napi::TypeError::New(env, "MakeCall() requires the second argument to be a number")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (!info[2].IsString())
  {
    Napi::TypeError::New(env, "MakeCall() requires the second argument to be a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  auto id = info[0].As<Napi::Number>().Int32Value();
  auto type = info[1].As<Napi::Number>().Int32Value();
  auto data = info[2].As<Napi::String>().Utf8Value();

  std::lock_guard<std::mutex> lock(events_queue_mutex_);
  events_queue_.push({id, type, data});
  return info.This();
}

Napi::Value UnityEventListenerWrap::Dispose(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  tsfn_.Unref(env);
  tsfn_.Release();
  instance_ = nullptr;
  disposed_ = true;
  dispatch_available_ = false;
  return info.This();
}
