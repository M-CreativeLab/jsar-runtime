#include "binding.hpp"

namespace bindings
{
  namespace env
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      EnvironmentWrap::Init(env, exports);
      ClientContext::Init(env, exports);
      return exports;
    }
  }
} // namespace bindings
