#pragma once

#include <memory>
#include <napi.h>
#include "client/browser/location.hpp"

namespace browserbinding
{
  class Location : public Napi::ObjectWrap<Location>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::string url);

  public:
    Location(const Napi::CallbackInfo &info);
    ~Location() = default;

  public:
    Napi::Value Assign(const Napi::CallbackInfo &info);
    Napi::Value Reload(const Napi::CallbackInfo &info);
    Napi::Value Replace(const Napi::CallbackInfo &info);
    Napi::Value ToString(const Napi::CallbackInfo &info);

  private:
    browser::Location locationImpl;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
