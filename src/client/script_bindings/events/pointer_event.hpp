#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/dom/events/pointer_event.hpp>
#include <client/script_bindings/event.hpp>
#include "./mouse_event.hpp"

namespace script_bindings
{
  /**
   * Event wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::Event objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM Event interface including properties like type, bubbles,
   * cancelable and methods like preventDefault().
   */
  class PointerEvent : public scripting_base::ObjectWrap<PointerEvent, dom::events::PointerEvent, UIEvent>
  {
    using scripting_base::ObjectWrap<PointerEvent, dom::events::PointerEvent, UIEvent>::ObjectWrap;

  public:
    static std::string Name()
    {
      return "PointerEvent";
    }
  };
}
