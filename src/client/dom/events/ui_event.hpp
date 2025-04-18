#pragma once

#include "../dom_event_target.hpp"

namespace dom::events
{
  class UIEvent : public dom::Event
  {
    using dom::Event::Event;

  protected:
    long detail_ = 0;
  };
}
