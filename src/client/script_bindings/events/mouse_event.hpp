#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/dom/events/mouse_event.hpp>
#include <client/script_bindings/event.hpp>

#include "./ui_event.hpp"

namespace script_bindings::event_bindings
{
  class MouseEvent;
  using MouseEventBase = scripting_base::ObjectWrap<MouseEvent, dom::events::MouseEvent, UIEvent>;

  /**
   * Event wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::Event objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM Event interface including properties like type, bubbles,
   * cancelable and methods like preventDefault().
   */
  class MouseEvent : public MouseEventBase
  {
    using MouseEventBase::ObjectWrap;

  public:
    static std::string Name()
    {
      return "MouseEvent";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                             std::shared_ptr<::dom::events::MouseEvent> nativeEvent);
  };
}
