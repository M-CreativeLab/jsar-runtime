#pragma once

#include <napi.h>
#include "unity_event_listener_wrap.hpp"
#include "event_target.hpp"

namespace bindings
{
  namespace messaging
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
