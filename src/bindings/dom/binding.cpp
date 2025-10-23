#include "./binding.hpp"
#include "./browsing_context.hpp"
#include "./worker_context.hpp"

namespace bindings
{
  namespace dom
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      dombinding::BrowsingContext::Init(env, exports);
      dombinding::WorkerContext::Init(env, exports);
      return exports;
    }
  }
}
