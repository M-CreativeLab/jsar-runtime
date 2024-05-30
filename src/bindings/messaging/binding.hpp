#pragma once

#include <napi.h>
#include "unity_event_listener_wrap.hpp"
#include "event_target.hpp"

namespace bindings
{
  Napi::Object InitMessagingModule(Napi::Env env, Napi::Object exports);
}
