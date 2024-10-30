#pragma once

#include <string>
#include <vector>
#include <memory>
#include "common/utility.hpp"
#include "common/events_v2/event_target.hpp"
#include "../dom/dom_event_target.hpp"

using namespace std;

namespace browser
{
  class Window : public dom::DOMEventTarget
  {
  public:
    /**
     * Create an empty `Node` object.
     */
    Window() : dom::DOMEventTarget() {}
  };
}
