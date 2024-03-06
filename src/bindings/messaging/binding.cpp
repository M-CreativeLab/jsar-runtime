#include "binding.hpp"

using namespace bindings;

Napi::Object bindings::InitMessagingModule(Napi::Env env, Napi::Object exports)
{
  messaging::UnityEventListenerWrap::Init(env, exports);
  return exports;
}
