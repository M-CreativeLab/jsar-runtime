#pragma once

#include <thread>
#include <napi.h>
#include "client/per_process.hpp"

namespace bindings
{
  namespace messaging
  {
    class NativeEventTarget : public Napi::ObjectWrap<NativeEventTarget>
    {
    public:
      static void Init(Napi::Env env, Napi::Object exports);

    public:
      NativeEventTarget(const Napi::CallbackInfo &info);
      ~NativeEventTarget();

    public:
      Napi::Value DispatchEvent(const Napi::CallbackInfo &info);

    private:
      TrClientContextPerProcess *clientContext;
      Napi::ThreadSafeFunction eventListener;
      thread eventRecvThread;
      bool recvingEvents = false;

    public:
      static Napi::FunctionReference *constructor;
    };
  }
}
