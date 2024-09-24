#pragma once

#include <napi.h>

namespace bindings
{
  namespace browser
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
