#include "binding.hpp"

namespace bindings
{
  Napi::Object InitEnvModule(Napi::Env env, Napi::Object exports)
  {
    EnvironmentWrap::Init(env, exports);
    return exports;
  }
} // namespace bindings
