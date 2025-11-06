#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/dom/events/wheel_event.hpp>
#include <client/script_bindings/event.hpp>

#include "./mouse_event.hpp"

namespace endor
{
  namespace script_bindings::event_bindings
  {
    class WheelEvent;
    using WheelEventBase = scripting_base::ObjectWrap<WheelEvent, dom::events::WheelEvent, MouseEvent>;

    /**
     * WheelEvent wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::events::WheelEvent objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM WheelEvent interface including deltaX, deltaY, deltaZ and deltaMode.
     */
    class WheelEvent : public WheelEventBase
    {
      using WheelEventBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "WheelEvent";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<::endor::dom::events::WheelEvent> nativeEvent);

    public:
      WheelEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void DeltaXGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void DeltaYGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void DeltaZGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void DeltaModeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
} // namespace endor
