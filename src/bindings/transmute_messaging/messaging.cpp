#include "messaging.hpp"

using namespace messaging;

Napi::Object messaging::InitMessagingModule(Napi::Env env, Napi::Object exports)
{
  UnityEventListenerWrap::Init(env, exports);
  return exports;
}
