#pragma once

#include "env_wrap.hpp"
#include "client_context.hpp"

namespace bindings
{
  namespace env
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
