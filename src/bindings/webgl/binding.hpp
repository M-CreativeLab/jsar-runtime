#pragma once

#include <napi.h>

namespace bindings
{
  namespace webgl
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}