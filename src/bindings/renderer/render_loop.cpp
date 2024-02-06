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
        {InstanceMethod("setFrameCallback", &RenderLoop::SetFrameCallback),
         InstanceMethod("setFrameFinished", &RenderLoop::SetFrameFinished),
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

    disposed_ = false;
    available_ = false; // wait for the frame callback to be set.
    s_instance = this;
  }

  Napi::Value RenderLoop::SetFrameCallback(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (disposed_)
    {
      Napi::TypeError::New(env, "UnityEventListener is already disposed").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    m_frameCallback = Napi::ThreadSafeFunction::New(
        env,
        info[0].As<Napi::Function>(),
        "RenderLoop",
        0,
        1,
        [](Napi::Env env)
        {
          fprintf(stderr, "RenderLoop::SetFrameCallback() finalizer is called\n");
        });
    available_ = true;
    return info.This();
  }

  Napi::Value RenderLoop::SetFrameFinished(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    RenderAPI::Get()->EndFrame();
    m_frameCallbackFinished = true;
    // m_cv.notify_one();
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

  void RenderLoop::blockingCallFrame()
  {
    unique_lock<mutex> lk(m_mutex);
    m_frameCallbackFinished = false;
    m_frameCallback.NonBlockingCall();
    // m_cv.wait(lk, [this] { return m_frameCallbackFinished; });
  }
}
