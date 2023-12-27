#include "env_wrap.hpp"
#include "env.hpp"
#include "bindings/transmute_vgom/buffer_writter.hpp"

using namespace std;
using namespace env;
using namespace gom;

Napi::FunctionReference *EnvironmentWrap::constructor;

void EnvironmentWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env,
      "Environment",
      {InstanceMethod("getReadyContext", &EnvironmentWrap::GetReadyContext),
       InstanceMethod("markRuntimeAvailable", &EnvironmentWrap::MarkRuntimeAvailable),
       InstanceMethod("markGomBufferAsErrored", &EnvironmentWrap::MarkGomBufferAsErrored)});

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

Napi::Value EnvironmentWrap::GetReadyContext(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  TransmuteEnvironment *transmuteEnv = TransmuteEnvironment::GetInstance();
  if (transmuteEnv == nullptr || !transmuteEnv->IsReady())
    return env.Null();

  return Napi::String::New(env, transmuteEnv->GetReadyContext());
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
  transmuteEnv->MarkRuntimeAvailable(runtimeVersions);
  return env.Undefined();
}

Napi::Value EnvironmentWrap::MarkGomBufferAsErrored(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::String channelIdString = info[0].As<Napi::String>();
  Napi::Number errorCodeNumber = info[1].As<Napi::Number>();
  std::string channelId = channelIdString.Utf8Value();

  if (GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()] == nullptr)
  {
    Napi::TypeError::New(env, "The GOM buffer writter is not found.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  GameObjectModelBufferWritter *writter = GameObjectModelBufferWritter::keyedBufferWrittersMap[channelId.c_str()];
  writter->setError(errorCodeNumber.Int32Value());
  return env.Undefined();
}