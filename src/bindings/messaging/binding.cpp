#include "binding.hpp"

using namespace bindings;

namespace bindings
{
  namespace messaging
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      NativeEventTarget::Init(env, exports);
      return exports;
    }
  } // namespace messaging
} // namespace bindings
