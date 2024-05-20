#include "render_loop.hpp"
#include "renderer/render_api.hpp"

namespace renderer
{
  RenderLoop *RenderLoop::s_instance;
  Napi::FunctionReference *RenderLoop::constructor;

  Napi::Object RenderLoop::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(
        env,
        "RenderLoop",
        {InstanceMethod("supportsWebGL2", &RenderLoop::SupportsWebGL2),
         InstanceMethod("setExceptionCallback", &RenderLoop::SetExceptionCallback),
         InstanceMethod("setFrameCallback", &RenderLoop::SetFrameCallback),
         InstanceMethod("dispose", &RenderLoop::Dispose)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("RenderLoop", tpl);
    return exports;
  }

  RenderLoop *RenderLoop::GetInstance()
  {
    if (s_instance == nullptr)
      return nullptr;
    else
      return s_instance;
  }

  RenderLoop::RenderLoop(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RenderLoop>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (s_instance != nullptr)
    {
      Napi::TypeError::New(env, "RenderLoop is already initialized").ThrowAsJavaScriptException();
      return;
    }

    renderApi = RenderAPI::Get();
    if (renderApi == nullptr)
    {
      Napi::TypeError::New(env, "Native renderer is not ready").ThrowAsJavaScriptException();
      return;
    }
    disposed_ = false;
    available_ = false; // wait for the frame callback to be set.
    s_instance = this;
  }

  Napi::Value RenderLoop::SupportsWebGL2(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, RenderAPI::Get()->SupportsWebGL2());
  }

  Napi::Value RenderLoop::SetExceptionCallback(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (disposed_)
    {
      Napi::TypeError::New(env, "RenderLoop() instance is already disposed")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsCallback = info[0].As<Napi::Function>();
    m_ExceptionCallback = Napi::ThreadSafeFunction::New(
        env,
        jsCallback,
        "RenderLoop_ExceptionCallback",
        0,
        1,
        [](Napi::Env env)
        {
          // TODO
        });
    return info.This();
  }

  Napi::Value RenderLoop::SetFrameCallback(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (disposed_)
    {
      Napi::TypeError::New(env, "RenderLoop() instance is already disposed")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsCallback = info[0].As<Napi::Function>();
    m_frameCallback = Napi::ThreadSafeFunction::New(
        env,
        jsCallback,
        "RenderLoop_FrameCallback",
        0,
        1,
        [](Napi::Env env)
        {
          // TODO
        });
    available_ = true;
    return info.This();
  }

  Napi::Value RenderLoop::Dispose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    m_frameCallback.Unref(env);
    m_frameCallback.Release();
    s_instance = NULL;
    disposed_ = true;
    available_ = false;
    return info.This();
  }

  void RenderLoop::reportException(uint32_t code)
  {
    m_ExceptionCallback.NonBlockingCall([code](Napi::Env env, Napi::Function jsCallback)
                                        { jsCallback.Call({Napi::Number::New(env, code)}); });
  }

  void RenderLoop::onAnimationFrame(chrono::time_point<chrono::high_resolution_clock> time)
  {
    unique_lock<mutex> lk(m_mutex);
    auto timeInMs = chrono::duration_cast<chrono::milliseconds>(time.time_since_epoch()).count();
    m_frameCallback.NonBlockingCall([timeInMs](Napi::Env env, Napi::Function jsCallback)
                                    { jsCallback.Call({Napi::Number::New(env, timeInMs)}); });
  }
}
