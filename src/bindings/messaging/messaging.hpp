#pragma once

#include "unity_event_listener_wrap.hpp"

namespace messaging
{
  Napi::Object InitMessagingModule(Napi::Env env, Napi::Object exports);
}
