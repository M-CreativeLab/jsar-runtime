#include "binding.hpp"
#include "device_native.hpp"

using namespace bindings;

Napi::Object bindings::InitWebxrModule(Napi::Env env, Napi::Object exports)
{
  bindings::XRDeviceNative::Init(env, exports);
  return exports;
}
