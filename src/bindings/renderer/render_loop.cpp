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
        {InstanceMethod("setExceptionCallback", &RenderLoop::SetExceptionCallback),
         InstanceMethod("setFrameCallback", &RenderLoop::SetFrameCallback),
         InstanceMethod("setFrameFinished", &RenderLoop::SetFrameFinished),
         InstanceMethod("getCommandBuffersCount", &RenderLoop::GetCommandBuffersCount),
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

  Napi::Value RenderLoop::SetFrameFinished(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    finished_ = true;
    return info.This();
  }

  Napi::Value RenderLoop::GetCommandBuffersCount(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, RenderAPI::Get()->GetCommandBuffersCount());
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

  void RenderLoop::frameCallback(xr::DeviceFrame *frame)
  {
    unique_lock<mutex> lk(m_mutex);
    m_frameCallbackFinished = false;
    m_frameCallback.NonBlockingCall(static_cast<void *>(frame), [](Napi::Env env, Napi::Function jsCallback, void *context)
                                    {
                                      auto device = xr::Device::GetInstance();
                                      if (device == nullptr)
                                      {
                                        // Invalid callback
                                        if (context != nullptr)
                                          delete static_cast<xr::DeviceFrame *>(context);
                                      }

                                      auto frame = static_cast<xr::DeviceFrame *>(context);
                                      auto time = Napi::Number::New(env, frame->getTimestamp());
                                      auto data = Napi::Object::New(env);

                                      // viewer transform
                                      auto viewerTransform = frame->getViewerTransform();
                                      auto jsViewerTransform = Napi::Float32Array::New(env, 16);
                                      for (int i = 0; i < 16; i++)
                                        jsViewerTransform.Set(i, Napi::Number::New(env, viewerTransform[i]));
                                      data.Set("viewerTransform", jsViewerTransform);

                                      // session-based fields `sessions`.
                                      auto jsItemsBySessionId = Napi::Array::New(env);
                                      int sessionIndex = 0;
                                      frame->iterateSessions([&](int sessionId, xr::FrameContextBySessionId *context)
                                                              {
                                                                auto jsSessionItem = Napi::Object::New(env);
                                                                auto localTransform = context->getLocalTransform();
                                                                auto jsLocalTransform = Napi::Float32Array::New(env, 16);
                                                                for (int i = 0; i < 16; i++)
                                                                  jsLocalTransform.Set(i, Napi::Number::New(env, localTransform[i]));
                                                                jsSessionItem.Set("sessionId", Napi::Number::New(env, sessionId));
                                                                jsSessionItem.Set("localTransform", jsLocalTransform);
                                                                jsItemsBySessionId.Set(sessionIndex++, jsSessionItem);
                                                              });
                                      data.Set("sessions", jsItemsBySessionId);

                                      // set current stereo id
                                      auto stereoId = Napi::Number::New(env, frame->getCurrentStereoId());
                                      data.Set("stereoId", stereoId);

                                      // add MultiPass frame data
                                      if (device->getStereoRenderingMode() == xr::StereoRenderingMode::MultiPass)
                                      {
                                        auto multipassFrame = static_cast<xr::MultiPassFrame *>(frame);
                                        auto activeEyeId = Napi::Number::New(env, multipassFrame->getActiveEyeId());
                                        data.Set("type", Napi::String::New(env, "MultiPassFrame"));
                                        data.Set("activeEyeId", activeEyeId);
                                        // viewer model matrix or transform
                                        // viewer view matrix
                                        auto viewMatrix = multipassFrame->getViewerViewMatrix();
                                        auto jsViewMatrix = Napi::Float32Array::New(env, 16);
                                        for (int i = 0; i < 16; i++)
                                          jsViewMatrix.Set(i, Napi::Number::New(env, viewMatrix[i]));
                                        data.Set("viewerViewMatrix", jsViewMatrix);
                                        // viewer projection matrix
                                        auto projectionMatrix = multipassFrame->getViewerProjectionMatrix();
                                        auto jsProjectionMatrix = Napi::Float32Array::New(env, 16);
                                        for (int i = 0; i < 16; i++)
                                          jsProjectionMatrix.Set(i, Napi::Number::New(env, projectionMatrix[i]));
                                        data.Set("viewerProjectionMatrix", jsProjectionMatrix);
                                      }
                                      // TODO: support single pass?

                                      jsCallback.Call({time, data});
                                      delete frame; });
  }

  void RenderLoop::frameCallback()
  {
    unique_lock<mutex> lk(m_mutex);
    m_frameCallbackFinished = false;
    m_frameCallback.NonBlockingCall();
  }
}
