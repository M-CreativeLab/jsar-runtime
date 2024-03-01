#pragma once

#include <napi.h>

using namespace std;

namespace bindings
{
  class EnvironmentWrap : public Napi::ObjectWrap<EnvironmentWrap>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    EnvironmentWrap(const Napi::CallbackInfo &info);

  public:
    void SetRuntimeInit(const char *contextJson);

  private:
    /**
     * Get the ready context.
     */
    Napi::Value GetRuntimeInit(const Napi::CallbackInfo &info);
    /**
     * Mark the runtime is available to execute xsml source.
     */
    Napi::Value MarkRuntimeAvailable(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };
} // namespace bindings
