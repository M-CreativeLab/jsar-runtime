#pragma once

#include "env_wrap.hpp"

namespace bindings
{
  Napi::Object InitEnvModule(Napi::Env env, Napi::Object exports);
}
