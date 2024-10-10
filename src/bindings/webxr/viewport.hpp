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

  private:
    Napi::Value WidthGetter(const Napi::CallbackInfo &info);
    Napi::Value HeightGetter(const Napi::CallbackInfo &info);
    Napi::Value XAxisGetter(const Napi::CallbackInfo &info);
    Napi::Value YAxisGetter(const Napi::CallbackInfo &info);

  public:
    TrViewport config;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
