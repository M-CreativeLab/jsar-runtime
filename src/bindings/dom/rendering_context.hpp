#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/rendering_context.hpp"
#include "./runtime_context.hpp"

namespace dombinding
{
  class DocumentRenderingContext : public RuntimeContextBase<DocumentRenderingContext, dom::DocumentRenderingContext>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    using RuntimeContextBase::RuntimeContextBase;

  private:
    Napi::Value Start(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
