#include "./xr_input_source_event.hpp"
#include <client/script_bindings/webxr/xr_frame.hpp>
#include <client/script_bindings/webxr/xr_input_source.hpp>

namespace script_bindings::event_bindings
{
  using namespace std;
  using namespace v8;

  // static
  void XRInputSourceEvent::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // Call parent ConfigureFunctionTemplate
    XRInputSourceEventBase::ConfigureFunctionTemplate(isolate, tpl);

    // Add XRInputSourceEvent-specific properties
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    instanceTemplate->SetAccessor(
      String::NewFromUtf8(isolate, "frame").ToLocalChecked(),
      FrameGetter,
      nullptr,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::DontDelete);

    instanceTemplate->SetAccessor(
      String::NewFromUtf8(isolate, "inputSource").ToLocalChecked(),
      InputSourceGetter,
      nullptr,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::DontDelete);
  }

  // static
  Local<Object> XRInputSourceEvent::NewInstance(Isolate *isolate, shared_ptr<client_xr::XRInputSourceEvent> nativeEvent)
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

  XRInputSourceEvent::XRInputSourceEvent(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : XRInputSourceEventBase(isolate, args)
  {
    // Constructor implementation - base class handles the native object wrapping
  }

  // static
  void XRInputSourceEvent::FrameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    XRInputSourceEvent *self = Unwrap(isolate, info.Holder());
    if (self && self->handle())
    {
      auto frame = self->handle()->frame();
      if (frame)
      {
        Local<Object> frameObj = webxr_bindings::XRFrame::GetOrNewInstance(isolate, frame);
        info.GetReturnValue().Set(frameObj);
        return;
      }
    }

    info.GetReturnValue().SetNull();
  }

  // static
  void XRInputSourceEvent::InputSourceGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    XRInputSourceEvent *self = Unwrap(isolate, info.Holder());
    if (self && self->handle())
    {
      auto inputSource = self->handle()->inputSource();
      if (inputSource)
      {
        Local<Object> inputSourceObj = webxr_bindings::XRInputSource::GetOrNewInstance(isolate, inputSource);
        info.GetReturnValue().Set(inputSourceObj);
        return;
      }
    }

    info.GetReturnValue().SetNull();
  }
}
