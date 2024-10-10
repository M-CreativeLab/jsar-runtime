#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <napi.h>

#include "client/per_process.hpp"
#include "common/frame_request/types.hpp"

using namespace std;
using namespace frame_request;

namespace bindings
{
  class AnimationFrameListener : public Napi::ObjectWrap<AnimationFrameListener>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    AnimationFrameListener(const Napi::CallbackInfo &info);
    ~AnimationFrameListener();

  private:
    Napi::Value Connect(const Napi::CallbackInfo &info);
    Napi::Value IsConnected(const Napi::CallbackInfo &info);

  private:
    void tick();
    void onFrameRequest();

  private:
    atomic<bool> connected = false;
    TrClientContextPerProcess *clientContext = nullptr;
    Napi::ThreadSafeFunction onframeTsfn;
    uv_loop_t *eventloop;
    uv_timer_t tickHandle;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
