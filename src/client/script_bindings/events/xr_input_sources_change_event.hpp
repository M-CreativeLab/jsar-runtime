#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/xr/webxr_session_events.hpp>
#include <client/script_bindings/event.hpp>

namespace script_bindings::event_bindings
{
  class XRInputSourcesChangeEvent;
  using XRInputSourcesChangeEventBase = scripting_base::ObjectWrap<XRInputSourcesChangeEvent,
                                                                   client_xr::XRInputSourcesChangeEvent,
                                                                   Event>;

  /**
   * XRInputSourcesChangeEvent wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps client_xr::XRInputSourcesChangeEvent objects for use in V8 JavaScript execution contexts.
   * It provides the standard XRInputSourcesChangeEvent interface including the session, added, and removed properties.
   */
  class XRInputSourcesChangeEvent : public XRInputSourcesChangeEventBase
  {
    using XRInputSourcesChangeEventBase::ObjectWrap;

  public:
    /**
     * The name of the XRInputSourcesChangeEvent class for V8.
     */
    static std::string Name()
    {
      return "XRInputSourcesChangeEvent";
    }

    /**
     * Configure the V8 function template with XRInputSourcesChangeEvent properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 XRInputSourcesChangeEvent instance from a native client_xr::XRInputSourcesChangeEvent.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRInputSourcesChangeEvent> nativeEvent);

  public:
    XRInputSourcesChangeEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Property getters
    static void SessionGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void AddedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void RemovedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  };
}