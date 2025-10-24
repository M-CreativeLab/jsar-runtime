#pragma once

#include <string>
#include <client/dom/dom_event_target.hpp>

namespace endor
{
  namespace client_workers
  {
    class WorkerGlobalScope : public dom::DOMEventTarget
    {
    public:
      WorkerGlobalScope();
    };
  }
} // namespace endor
