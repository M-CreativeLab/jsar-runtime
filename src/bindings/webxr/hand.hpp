#pragma once

#include <map>
#include <napi.h>
#include "common/xr/types.hpp"
#include "./common.hpp"

namespace bindings
{
  class XRHand : public Napi::ObjectWrap<XRHand>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, xr::TrXRInputSource *inputSourceInternal);
    XRHand(Napi::CallbackInfo const &info);
    ~XRHand();

  private:
    Napi::Value Entries(Napi::CallbackInfo const &info);
    Napi::Value ForEach(Napi::CallbackInfo const &info);
    Napi::Value Get(Napi::CallbackInfo const &info);
    Napi::Value Keys(Napi::CallbackInfo const &info);
    Napi::Value Values(Napi::CallbackInfo const &info);

  private:
    xr::TrXRInputSource *internal = nullptr;
    std::map<std::string, Napi::ObjectReference*> entries;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
