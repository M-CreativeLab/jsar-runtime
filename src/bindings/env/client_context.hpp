#pragma once

#include <napi.h>
#include "client/per_process.hpp"

using namespace std;

namespace bindings
{
  class ClientContext : public Napi::ObjectWrap<ClientContext>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    ClientContext(const Napi::CallbackInfo &info);

  private:
    Napi::Value KeepAlive(const Napi::CallbackInfo &info);
    Napi::Value FastPerformanceNow(const Napi::CallbackInfo &info);

  private:
    TrClientContextPerProcess *clientContext = nullptr;

  private:
    static Napi::FunctionReference *constructor;
  };
} // namespace bindings
