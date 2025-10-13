#pragma once

#include <memory>
#include <napi.h>
#include <client/per_process.hpp>
#include <client/dom/browsing_context.hpp>
#include "./runtime_context.hpp"

namespace dombinding
{
  class BrowsingContext : public RuntimeContextBase<BrowsingContext, dom::BrowsingContext>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    BrowsingContext(const Napi::CallbackInfo &info);

  private:
    Napi::Value Start(const Napi::CallbackInfo &info);

  private:
    TrClientContextPerProcess *client_context_;
    static thread_local Napi::FunctionReference *constructor;
  };
}
