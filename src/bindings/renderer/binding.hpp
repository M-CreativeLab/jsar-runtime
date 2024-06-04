#pragma once

#include <napi.h>

namespace bindings
{
  namespace renderer
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
