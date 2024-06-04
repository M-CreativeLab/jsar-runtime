#pragma once

#include <napi.h>

namespace bindings
{
  namespace webxr
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
