#pragma once

#include <napi.h>
#include "event_target.hpp"

namespace bindings
{
  namespace messaging
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
