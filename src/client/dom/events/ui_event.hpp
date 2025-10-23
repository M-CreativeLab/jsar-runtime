#pragma once

#include "../dom_event.hpp"
#include "../dom_event_target.hpp"

namespace dom::events
{
  class UIEvent : public dom::Event
  {
    using dom::Event::Event;

  public:
    bool isUIEvent() const override final
    {
      return true;
    }

  protected:
    long detail_ = 0;
  };
}
