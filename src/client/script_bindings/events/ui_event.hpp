#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/dom/events/ui_event.hpp>
#include <client/script_bindings/event.hpp>

namespace script_bindings::event_bindings
{
  class UIEvent;
  using UIEventBase = scripting_base::ObjectWrap<UIEvent, dom::events::UIEvent, Event>;

  /**
   * Event wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::Event objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM Event interface including properties like type, bubbles,
   * cancelable and methods like preventDefault().
   */
  class UIEvent : public UIEventBase
  {
    using UIEventBase::ObjectWrap;

  public:
    /**
     * The name of the Event class for V8.
     */
    static std::string Name()
    {
      return "UIEvent";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                             std::shared_ptr<::dom::events::UIEvent> nativeEvent);

  private:
    // Property getters
    void DetailGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
  };
}
