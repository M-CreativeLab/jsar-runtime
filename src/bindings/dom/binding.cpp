#include "binding.hpp"
#include "./audio.hpp"

namespace bindings
{
  namespace dom
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      Audio::Init(env, exports);
      return exports;
    }
  }
}
