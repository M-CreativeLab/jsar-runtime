#pragma once

#include <napi.h>
#include <node/v8.h>
#include <client/dom/intersection_observer.hpp>

namespace dombinding
{
  class IntersectionObserver : public Napi::ObjectWrap<IntersectionObserver>
  {
  public:
    static void Init(Napi::Env env);
    static void MicrotaskHandler(v8::Isolate *isolate, void *data);

  public:
    IntersectionObserver(const Napi::CallbackInfo &info);
    ~IntersectionObserver();

  private:
    Napi::Value Disconnect(const Napi::CallbackInfo &info);
    Napi::Value Observe(const Napi::CallbackInfo &info);
    Napi::Value Unobserve(const Napi::CallbackInfo &info);
    Napi::Value TakeRecords(const Napi::CallbackInfo &info);

    // Property getters
    Napi::Value GetRoot(const Napi::CallbackInfo &info);
    Napi::Value GetRootMargin(const Napi::CallbackInfo &info);
    Napi::Value GetThresholds(const Napi::CallbackInfo &info);

  private:
    void dispatchEntriesCallback(Napi::Env env);
    void startEntriesDispatcher(Napi::Env env);
    void stopEntriesDispatcher();

  private:
    napi_env callbackEnv_;
    Napi::FunctionReference callbackRef_;
    std::shared_ptr<dom::IntersectionObserver> observer_ = nullptr;

  private:
    thread_local static Napi::FunctionReference *constructor;
  };
} // namespace dombinding