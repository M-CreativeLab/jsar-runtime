#pragma once

#include <napi.h>

namespace bindings
{
  namespace dom
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
