#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <napi.h>

#include "xr/frame.hpp"
#include "renderer/render_api.hpp"

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

    /**
     * Report a native exception to JavaScript side.
     */
    void reportException(uint32_t code);
    /**
     * Call the frame callback that is set in JavaScript runtime.
     */
    void onAnimationFrame(chrono::time_point<chrono::high_resolution_clock> now);

  private:
    Napi::Value SupportsWebGL2(const Napi::CallbackInfo &info);
    Napi::Value SetExceptionCallback(const Napi::CallbackInfo &info);
    Napi::Value SetFrameCallback(const Napi::CallbackInfo &info);
    Napi::Value SetFrameFinished(const Napi::CallbackInfo &info);
    Napi::Value Dispose(const Napi::CallbackInfo &info);

  private:
    atomic<bool> disposed_ = true;
    atomic<bool> available_ = false;
    RenderAPI *renderApi = nullptr;

    // thread safe controls
    mutex m_mutex;
    condition_variable m_cv;
    Napi::ThreadSafeFunction m_ExceptionCallback;
    Napi::ThreadSafeFunction m_frameCallback;

  private:
    static RenderLoop *s_instance;
    static Napi::FunctionReference *constructor;
  };
}
