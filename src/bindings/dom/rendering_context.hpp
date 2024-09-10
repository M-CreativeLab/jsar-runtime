#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/rendering_context.hpp"

namespace dombinding
{
  class DocumentRenderingContext : public Napi::ObjectWrap<DocumentRenderingContext>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    DocumentRenderingContext(const Napi::CallbackInfo &info);
    ~DocumentRenderingContext();

  private:
    Napi::Value SetResourceLoader(const Napi::CallbackInfo &info);
    Napi::Value Start(const Napi::CallbackInfo &info);

  private:
    shared_ptr<dom::DocumentRenderingContext> renderingContext;

  private:
    static Napi::FunctionReference *constructor;
  };
}
