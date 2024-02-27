#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <napi.h>

using namespace std;

namespace renderer
{
  class RenderLoop : public Napi::ObjectWrap<RenderLoop>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static RenderLoop *GetInstance();
    RenderLoop(const Napi::CallbackInfo &info);

  public:
    bool isAvailable() { return disposed_ == false && available_ == true; }
    Napi::ThreadSafeFunction &getFrameCallback() { return m_frameCallback; }
    void blockingCallFrame();
    void startFrame() { finished_ = false; }
    bool isFrameFinished() { return finished_; }

  private:
    Napi::Value SetFrameCallback(const Napi::CallbackInfo &info);
    Napi::Value SetFrameFinished(const Napi::CallbackInfo &info);
    Napi::Value GetCommandBuffersCount(const Napi::CallbackInfo &info);
    Napi::Value Dispose(const Napi::CallbackInfo &info);

  private:
    atomic<bool> disposed_ = true;
    atomic<bool> available_ = false;
    atomic<bool> finished_ = false;

    Napi::ThreadSafeFunction m_frameCallback;
    mutex m_mutex;
    condition_variable m_cv;
    bool m_frameCallbackFinished = true;

    static RenderLoop *s_instance;
    static Napi::FunctionReference *constructor;
  };
}
