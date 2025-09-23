#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/xr/webxr_session_events.hpp>
#include <client/script_bindings/event.hpp>

namespace script_bindings::event_bindings
{
  class XRInputSourceEvent;
  using XRInputSourceEventBase = scripting_base::ObjectWrap<XRInputSourceEvent, client_xr::XRInputSourceEvent, Event>;

  /**
   * XRInputSourceEvent wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps client_xr::XRInputSourceEvent objects for use in V8 JavaScript execution contexts.
   * It provides the standard XRInputSourceEvent interface including the frame and inputSource properties.
   */
  class XRInputSourceEvent : public XRInputSourceEventBase
  {
    using XRInputSourceEventBase::ObjectWrap;

  public:
    /**
     * The name of the XRInputSourceEvent class for V8.
     */
    static std::string Name()
    {
      return "XRInputSourceEvent";
    }

    /**
     * Configure the V8 function template with XRInputSourceEvent properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 XRInputSourceEvent instance from a native client_xr::XRInputSourceEvent.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRInputSourceEvent> nativeEvent);

  public:
    XRInputSourceEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Property getters
    static void FrameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void InputSourceGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  };
}