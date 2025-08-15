#include "binding.hpp"
#include "./location.hpp"
#include "./window.hpp"
#include "./navigator.hpp"

namespace bindings
{
  namespace browser
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      browserbinding::Location::Init(env);
      browserbinding::Window::Init(env);
      Navigator::Init(env, exports);
      return exports;
    }
  }
}
