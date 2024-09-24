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
    ~Location();

  public:
    Napi::Value assign(const Napi::CallbackInfo &info);
    Napi::Value reload(const Napi::CallbackInfo &info);
    Napi::Value replace(const Napi::CallbackInfo &info);
    Napi::Value toString(const Napi::CallbackInfo &info);

  private:
    browser::Location locationImpl;

  public:
    static Napi::FunctionReference *constructor;
  };
}
