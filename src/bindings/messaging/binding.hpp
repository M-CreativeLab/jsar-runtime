#pragma once

#include <napi.h>
#include "unity_event_listener_wrap.hpp"

namespace bindings
{
  Napi::Object InitMessagingModule(Napi::Env env, Napi::Object exports);
}
