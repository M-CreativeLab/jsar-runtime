#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <napi.h>
#include "xr/frame.hpp"

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
    void startFrame() { finished_ = false; }
    bool isFrameFinished() { return finished_; }

    /**
     * Report a native exception to JavaScript side.
     */
    void reportException(uint32_t code);
    /**
     * Call the frame callback that is set in JavaScript runtime.
     */
    void frameCallback(xr::DeviceFrame *frame);
    void frameCallback();

  private:
    Napi::Value SetExceptionCallback(const Napi::CallbackInfo &info);
    Napi::Value SetFrameCallback(const Napi::CallbackInfo &info);
    Napi::Value SetFrameFinished(const Napi::CallbackInfo &info);
    Napi::Value GetCommandBuffersCount(const Napi::CallbackInfo &info);
    Napi::Value Dispose(const Napi::CallbackInfo &info);

  private:
    atomic<bool> disposed_ = true;
    atomic<bool> available_ = false;
    atomic<bool> finished_ = false;

    mutex m_mutex;
    condition_variable m_cv;
    bool m_frameCallbackFinished = true;
    Napi::ThreadSafeFunction m_ExceptionCallback;
    Napi::ThreadSafeFunction m_frameCallback;

    static RenderLoop *s_instance;
    static Napi::FunctionReference *constructor;
  };
}
