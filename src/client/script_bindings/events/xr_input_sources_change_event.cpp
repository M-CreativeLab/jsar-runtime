#include "./xr_input_sources_change_event.hpp"
#include <client/script_bindings/webxr/xr_session.hpp>
#include <client/script_bindings/webxr/xr_input_source.hpp>

namespace script_bindings::event_bindings
{
  using namespace std;
  using namespace v8;

  // static
  void XRInputSourcesChangeEvent::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // Call parent ConfigureFunctionTemplate
    XRInputSourcesChangeEventBase::ConfigureFunctionTemplate(isolate, tpl);

    // Add XRInputSourcesChangeEvent-specific properties
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    instanceTemplate->SetAccessor(
      String::NewFromUtf8(isolate, "session").ToLocalChecked(),
      SessionGetter,
      nullptr,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::DontDelete);

    instanceTemplate->SetAccessor(
      String::NewFromUtf8(isolate, "added").ToLocalChecked(),
      AddedGetter,
      nullptr,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::DontDelete);

    instanceTemplate->SetAccessor(
      String::NewFromUtf8(isolate, "removed").ToLocalChecked(),
      RemovedGetter,
      nullptr,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::DontDelete);
  }

  // static
  Local<Object> XRInputSourcesChangeEvent::NewInstance(Isolate *isolate, shared_ptr<client_xr::XRInputSourcesChangeEvent> nativeEvent)
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

  XRInputSourcesChangeEvent::XRInputSourcesChangeEvent(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : XRInputSourcesChangeEventBase(isolate, args)
  {
    // Constructor implementation - base class handles the native object wrapping
  }

  // static
  void XRInputSourcesChangeEvent::SessionGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    XRInputSourcesChangeEvent *self = Unwrap(isolate, info.Holder());
    if (self && self->handle())
    {
      auto session = self->handle()->session();
      if (session)
      {
        Local<Object> sessionObj = webxr_bindings::XRSession::GetOrNewInstance(isolate, session);
        info.GetReturnValue().Set(sessionObj);
        return;
      }
    }

    info.GetReturnValue().SetNull();
  }

  // static
  void XRInputSourcesChangeEvent::AddedGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    XRInputSourcesChangeEvent *self = Unwrap(isolate, info.Holder());
    if (self && self->handle())
    {
      auto addedSources = self->handle()->added();
      Local<Array> array = Array::New(isolate, addedSources.size());

      for (size_t i = 0; i < addedSources.size(); ++i)
      {
        Local<Object> inputSourceObj = webxr_bindings::XRInputSource::GetOrNewInstance(isolate, addedSources[i]);
        array->Set(isolate->GetCurrentContext(), i, inputSourceObj).Check();
      }

      info.GetReturnValue().Set(array);
      return;
    }

    info.GetReturnValue().Set(Array::New(isolate, 0));
  }

  // static
  void XRInputSourcesChangeEvent::RemovedGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    XRInputSourcesChangeEvent *self = Unwrap(isolate, info.Holder());
    if (self && self->handle())
    {
      auto removedSources = self->handle()->removed();
      Local<Array> array = Array::New(isolate, removedSources.size());

      for (size_t i = 0; i < removedSources.size(); ++i)
      {
        Local<Object> inputSourceObj = webxr_bindings::XRInputSource::GetOrNewInstance(isolate, removedSources[i]);
        array->Set(isolate->GetCurrentContext(), i, inputSourceObj).Check();
      }

      info.GetReturnValue().Set(array);
      return;
    }

    info.GetReturnValue().Set(Array::New(isolate, 0));
  }
}
