#pragma once

#include "env_wrap.hpp"
#include "client_context.hpp"

namespace bindings
{
  Napi::Object InitEnvModule(Napi::Env env, Napi::Object exports);
}
