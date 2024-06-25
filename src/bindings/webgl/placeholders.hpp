#pragma once

#include <napi.h>

namespace bindings
{
  namespace webgl
  {
    void InitPlaceholders(Napi::Env env, Napi::Object exports);
  }
}
