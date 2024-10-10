#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/worker_context.hpp"
#include "./runtime_context.hpp"

namespace dombinding
{
  class WorkerContext : public RuntimeContextBase<WorkerContext, dom::WorkerContext>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    WorkerContext(const Napi::CallbackInfo &info);

  private:
    Napi::Value Start(const Napi::CallbackInfo &info);
    Napi::Value StartFromSource(const Napi::CallbackInfo &info);
    Napi::Value DispatchEvent(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
