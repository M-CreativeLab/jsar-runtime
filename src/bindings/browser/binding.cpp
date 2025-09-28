#include "binding.hpp"
#include "./location.hpp"

namespace bindings
{
  namespace browser
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      browserbinding::Location::Init(env);
      return exports;
    }
  }
}
