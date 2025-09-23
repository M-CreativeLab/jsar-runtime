#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/xr/webxr_session_events.hpp>
#include <client/script_bindings/event.hpp>

namespace script_bindings::event_bindings
{
  class XRSessionEvent;
  using XRSessionEventBase = scripting_base::ObjectWrap<XRSessionEvent, client_xr::XRSessionEvent, Event>;

  /**
   * XRSessionEvent wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps client_xr::XRSessionEvent objects for use in V8 JavaScript execution contexts.
   * It provides the standard XRSessionEvent interface including the session property.
   */
  class XRSessionEvent : public XRSessionEventBase
  {
    using XRSessionEventBase::ObjectWrap;

  public:
    /**
     * The name of the XRSessionEvent class for V8.
     */
    static std::string Name()
    {
      return "XRSessionEvent";
    }

    /**
     * Configure the V8 function template with XRSessionEvent properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 XRSessionEvent instance from a native client_xr::XRSessionEvent.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRSessionEvent> nativeEvent);

  public:
    XRSessionEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Property getters
    static void SessionGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  };
}