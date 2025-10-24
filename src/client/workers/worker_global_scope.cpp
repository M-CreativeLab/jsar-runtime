#include "./worker_global_scope.hpp"

namespace endor
{
  namespace client_workers
  {
    WorkerGlobalScope::WorkerGlobalScope()
        : dom::DOMEventTarget()
    {
    }
  }
} // namespace endor
