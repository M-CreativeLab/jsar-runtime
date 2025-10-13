#include <client/script_bindings/webxr/xr_session.hpp>
#include <client/script_bindings/webxr/xr_input_source.hpp>
#include "./xr_input_sources_change_event.hpp"

namespace script_bindings::event_bindings
{
  using namespace std;
  using namespace v8;

  // static
  void XRInputSourcesChangeEvent::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    InstanceReadonlyAccessor(isolate, instanceTemplate, "session", &XRInputSourcesChangeEvent::SessionGetter);
    InstanceReadonlyAccessor(isolate, instanceTemplate, "added", &XRInputSourcesChangeEvent::AddedGetter);
    InstanceReadonlyAccessor(isolate, instanceTemplate, "removed", &XRInputSourcesChangeEvent::RemovedGetter);
  }

  void XRInputSourcesChangeEvent::SessionGetter(const PropertyCallbackInfo<Value> &info)
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

  void XRInputSourcesChangeEvent::AddedGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    const auto& added = handle()->added();
    Local<Array> array = Array::New(isolate, added.size());
    for (size_t i = 0; i < added.size(); ++i)
    {
      Local<Object> inputSourceObj = webxr_bindings::XRInputSource::GetOrNewInstance(isolate, added[i]);
      array->Set(context, i, inputSourceObj).Check();
    }
    info.GetReturnValue().Set(array);
  }

  void XRInputSourcesChangeEvent::RemovedGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    const auto& removed = handle()->removed();
    Local<Array> array = Array::New(isolate, removed.size());
    for (size_t i = 0; i < removed.size(); ++i)
    {
      Local<Object> inputSourceObj = webxr_bindings::XRInputSource::GetOrNewInstance(isolate, removed[i]);
      array->Set(context, i, inputSourceObj).Check();
    }
    info.GetReturnValue().Set(array);
  }
}
