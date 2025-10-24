#include <client/script_bindings/webxr/xr_session.hpp>
#include "./xr_session_event.hpp"

namespace endor
{
  namespace script_bindings::event_bindings
  {
    using namespace std;
    using namespace v8;

    // static
    void XRSessionEvent::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instanceTemplate = tpl->PrototypeTemplate();

      // Add XRSessionEvent-specific properties
      InstanceReadonlyAccessor(isolate, instanceTemplate, "session", &XRSessionEvent::SessionGetter);
    }

    void XRSessionEvent::SessionGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto session = handle()->session();
      if (session)
      {
        Local<Object> sessionObj = webxr_bindings::XRSession::GetOrNewInstance(isolate, session);
        info.GetReturnValue().Set(sessionObj);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }
  }
} // namespace endor
