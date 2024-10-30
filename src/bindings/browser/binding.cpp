#include "binding.hpp"
#include "./location.hpp"
#include "./window.hpp"

namespace bindings
{
  namespace browser
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      browserbinding::Location::Init(env);
      browserbinding::Window::Init(env);
      return exports;
    }
  }
}
