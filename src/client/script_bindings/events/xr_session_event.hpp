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
    class XRSessionEvent;
    using XRSessionEventBase = scripting_base::ObjectWrap<XRSessionEvent,
                                                          client_xr::XRSessionEvent,
                                                          Event>;

    class XRSessionEvent : public XRSessionEventBase
    {
      using XRSessionEventBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRSessionEvent";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    private:
      // Property getters
      void SessionGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
} // namespace endor
