#pragma once

#include <map>
#include <optional>
#include <napi.h>
#include <common/xr/types.hpp>
#include <client/xr/webxr_hand.hpp>
#include "./common.hpp"

namespace bindings
{
  class XRHand : public Napi::ObjectWrap<XRHand>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRInputSource> inputSource);

  public:
    XRHand(Napi::CallbackInfo const &info);
    ~XRHand();

  private:
    Napi::Value Entries(Napi::CallbackInfo const &info);
    Napi::Value ForEach(Napi::CallbackInfo const &info);
    Napi::Value Get(Napi::CallbackInfo const &info);
    Napi::Value Keys(Napi::CallbackInfo const &info);
    Napi::Value Values(Napi::CallbackInfo const &info);

  private:
    std::optional<client_xr::XRHand> handle_;
    std::map<std::string, Napi::ObjectReference *> entries_;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
