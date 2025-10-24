#pragma once

#include <string>
#include <client/dom/dom_event_target.hpp>

namespace endor
{
  namespace client_workers
  {
    class Worker : public dom::DOMEventTarget
    {
    public:
      Worker(const std::string &url);
    };
  }
} // namespace endor
