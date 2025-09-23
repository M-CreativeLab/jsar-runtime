#include "./xr_session_event.hpp"
#include <client/script_bindings/webxr/xr_session.hpp>

namespace script_bindings::event_bindings
{
  using namespace std;
  using namespace v8;

  // static
  void XRSessionEvent::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // Call parent ConfigureFunctionTemplate
    XRSessionEventBase::ConfigureFunctionTemplate(isolate, tpl);

    // Add XRSessionEvent-specific properties
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
    instanceTemplate->SetAccessor(
      String::NewFromUtf8(isolate, "session").ToLocalChecked(),
      SessionGetter,
      nullptr,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::DontDelete);
  }

  // static
  Local<Object> XRSessionEvent::NewInstance(Isolate *isolate, shared_ptr<client_xr::XRSessionEvent> nativeEvent)
  {
    EscapableHandleScope scope(isolate);

    if (!GetConstructorFunction(isolate).IsEmpty())
    {
      Local<Function> constructor = GetConstructorFunction(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      // Create external wrapper for the native event
      Local<External> external = External::New(isolate, nativeEvent.get());
      Local<Value> args[] = {external};

      Local<Object> instance = constructor->NewInstance(context, 1, args).ToLocalChecked();
      return scope.Escape(instance);
    }

    return scope.Escape(Local<Object>());
  }

  XRSessionEvent::XRSessionEvent(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : XRSessionEventBase(isolate, args)
  {
    // Constructor implementation - base class handles the native object wrapping
  }

  // static
  void XRSessionEvent::SessionGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    XRSessionEvent *self = Unwrap(info.Holder());
    if (self && self->handle_)
    {
      auto session = self->handle_->session();
      if (session)
      {
        Local<Object> sessionObj = script_bindings::XRSession::NewInstance(isolate, session);
        info.GetReturnValue().Set(sessionObj);
        return;
      }
    }

    info.GetReturnValue().SetNull();
  }
}