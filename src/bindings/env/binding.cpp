#include "binding.hpp"

using namespace env;

Napi::Object env::InitEnvModule(Napi::Env env, Napi::Object exports)
{
  EnvironmentWrap::Init(env, exports);
  return exports;
}
