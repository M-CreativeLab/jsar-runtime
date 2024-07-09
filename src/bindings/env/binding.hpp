#pragma once

#include "client_context.hpp"

namespace bindings
{
  namespace env
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
