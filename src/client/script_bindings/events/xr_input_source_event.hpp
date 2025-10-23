#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/xr/webxr_session_events.hpp>
#include <client/script_bindings/event.hpp>

namespace script_bindings::event_bindings
{
  class XRInputSourceEvent;
  using XRInputSourceEventBase = scripting_base::ObjectWrap<XRInputSourceEvent,
                                                            client_xr::XRInputSourceEvent,
                                                            Event>;

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
    static std::string Name()
    {
      return "XRInputSourceEvent";
    }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

  private:
    // Property getters
    void FrameGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void InputSourceGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
  };
}
