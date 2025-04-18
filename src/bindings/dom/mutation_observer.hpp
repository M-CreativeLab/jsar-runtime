#pragma once

#include <napi.h>
#include <node/v8.h>
#include <client/dom/mutation_observer.hpp>

namespace dombinding
{
  class MutationObserver : public Napi::ObjectWrap<MutationObserver>
  {
  public:
    static void Init(Napi::Env env);
    static void MicrotaskHandler(v8::Isolate *isolate, void *data);

  public:
    MutationObserver(const Napi::CallbackInfo &info);
    ~MutationObserver();

  private:
    Napi::Value Disconnect(const Napi::CallbackInfo &info);
    Napi::Value Observe(const Napi::CallbackInfo &info);
    Napi::Value TakeRecords(const Napi::CallbackInfo &info);

  private:
    void dispatchRecordsCallback(Napi::Env env);
    void startRecordsDispatcher(Napi::Env env);
    void stopRecordsDispatcher();

  private:
    napi_env callbackEnv_;
    Napi::FunctionReference callbackRef_;
    std::shared_ptr<dom::MutationObserver> observer_ = nullptr;

  private:
    thread_local static Napi::FunctionReference *constructor;
  };
} // namespace dombinding
