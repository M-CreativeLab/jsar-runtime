#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/xr/webxr_session_events.hpp>
#include <client/script_bindings/event.hpp>

namespace endor
{
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
      static std::string Name()
      {
        return "XRInputSourcesChangeEvent";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    private:
      // Property getters
      void SessionGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void AddedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void RemovedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
} // namespace endor
