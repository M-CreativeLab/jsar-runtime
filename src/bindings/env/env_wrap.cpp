#include "env_wrap.hpp"
#include "env.hpp"

using namespace std;
using namespace bindings;

Napi::FunctionReference *EnvironmentWrap::constructor;

void EnvironmentWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env,
      "TransmuteEnvironment",
      {InstanceMethod("getRuntimeInit", &EnvironmentWrap::GetRuntimeInit),
       InstanceMethod("markRuntimeAvailable", &EnvironmentWrap::MarkRuntimeAvailable),});

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  env.SetInstanceData(constructor);

  exports.Set("Environment", tpl);
}

EnvironmentWrap::EnvironmentWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<EnvironmentWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
}

Napi::Value EnvironmentWrap::GetRuntimeInit(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  TransmuteEnvironment *transmuteEnv = TransmuteEnvironment::GetInstance();
  if (transmuteEnv == nullptr)
    return env.Null();

  return Napi::String::New(env, transmuteEnv->getRuntimeInit());
}

Napi::Value EnvironmentWrap::MarkRuntimeAvailable(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "The first argument must be a string.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  TransmuteEnvironment *transmuteEnv = TransmuteEnvironment::GetInstance();
  if (transmuteEnv == nullptr)
  {
    Napi::TypeError::New(env, "The environment is invalid, `TransmuteEnvironment` not initialized.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  string runtimeVersions = info[0].As<Napi::String>().Utf8Value();
  transmuteEnv->markRuntimeAvailable(runtimeVersions);
  return env.Undefined();
}
