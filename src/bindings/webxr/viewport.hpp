#pragma once

#include <napi.h>
#include "common.hpp"
#include "common/viewport.hpp"

namespace bindings
{
  class XRViewport : public Napi::ObjectWrap<XRViewport>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, TrViewport viewport);
    XRViewport(const Napi::CallbackInfo &info);

  public:
    TrViewport config;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
