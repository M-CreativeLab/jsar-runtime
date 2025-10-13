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
    HandleScope scope(isolate);
    Local<ObjectTemplate> instanceTemplate = tpl->PrototypeTemplate();

    // Add XRInputSourceEvent-specific properties
    InstanceReadonlyAccessor(isolate, instanceTemplate, "frame", &XRInputSourceEvent::FrameGetter);
    InstanceReadonlyAccessor(isolate, instanceTemplate, "inputSource", &XRInputSourceEvent::InputSourceGetter);
  }


  // static
  void XRInputSourceEvent::FrameGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto frame = handle()->frame();
    if (frame)
    {
      Local<Object> frameObj = webxr_bindings::XRFrame::GetOrNewInstance(isolate, frame);
      info.GetReturnValue().Set(frameObj);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  // static
  void XRInputSourceEvent::InputSourceGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto inputSource = handle()->inputSource();
    if (inputSource)
    {
      Local<Object> inputSourceObj = webxr_bindings::XRInputSource::GetOrNewInstance(isolate, inputSource);
      info.GetReturnValue().Set(inputSourceObj);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }
}
