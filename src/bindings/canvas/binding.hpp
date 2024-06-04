#pragma once

#include <napi.h>

namespace bindings
{
  namespace canvas
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
