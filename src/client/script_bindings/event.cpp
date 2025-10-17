#include <iostream>
#include "./event.hpp"
#include "./events/all_events.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  // static
  void Event::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);

    // Set up the instance template
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    // Add property accessors
    InstanceReadonlyPropertyAccessor(isolate, prototype, "type", &Event::TypeGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "bubbles", &Event::BubblesGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "cancelable", &Event::CancelableGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "composed", &Event::ComposedGetter);

    // Add methods
    InstanceMethod(isolate, prototype, "preventDefault", &Event::PreventDefault);
    InstanceMethod(isolate, prototype, "stopPropagation", &Event::StopPropagation);
  }

  // static
  Local<Object> Event::NewInstance(Isolate *isolate, std::shared_ptr<dom::Event> event)
  {
    EscapableHandleScope scope(isolate);
    assert(event != nullptr && "nativeEvent must not be null");

#define CHECK_AND_RETURN_INSTANCE_OF(EVENT_TYPE, HANDLE_TYPE)                          \
  if (event->is##EVENT_TYPE())                                                         \
  {                                                                                    \
    auto typedEvent = static_pointer_cast<HANDLE_TYPE>(event);                         \
    return scope.Escape(event_bindings::EVENT_TYPE::NewInstance(isolate, typedEvent)); \
  }
#define CHECK_AND_RETURN_INSTANCE_OF_DOM_EVENT(NAME) \
  CHECK_AND_RETURN_INSTANCE_OF(NAME, dom::events::NAME)
#define CHECK_AND_RETURN_INSTANCE_OF_XR_EVENT(NAME) \
  CHECK_AND_RETURN_INSTANCE_OF(NAME, client_xr::NAME)

    // DOM Events
    CHECK_AND_RETURN_INSTANCE_OF_DOM_EVENT(MessageEvent)
    CHECK_AND_RETURN_INSTANCE_OF_DOM_EVENT(PointerEvent)
    CHECK_AND_RETURN_INSTANCE_OF_DOM_EVENT(MouseEvent)
    CHECK_AND_RETURN_INSTANCE_OF_DOM_EVENT(UIEvent)

    // XR Events
    CHECK_AND_RETURN_INSTANCE_OF_XR_EVENT(XRSessionEvent)
    CHECK_AND_RETURN_INSTANCE_OF_XR_EVENT(XRInputSourceEvent)
    CHECK_AND_RETURN_INSTANCE_OF_XR_EVENT(XRInputSourcesChangeEvent)

#undef CHECK_AND_RETURN_INSTANCE_OF_DOM_EVENT
#undef CHECK_AND_RETURN_INSTANCE_OF_XR_EVENT
#undef CHECK_AND_RETURN_INSTANCE_OF

    // For other event types, use the base Event class
    return scope.Escape(EventBase::NewInstance(isolate, event).As<Object>());
  }

  Event::Event(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : EventBase(isolate, args)
  {
  }

  // Property getters

  void Event::TypeGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    string type = handle()->typeStr();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  type.c_str())
                                .ToLocalChecked());
  }

  void Event::BubblesGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate,
                                           handle()->bubbles()));
  }

  void Event::CancelableGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate,
                                           handle()->cancelable()));
  }

  void Event::ComposedGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate,
                                           handle()->composed()));
  }

  // Methods

  void Event::PreventDefault(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    handle()->preventDefault();
    info.GetReturnValue().SetUndefined();
  }

  void Event::StopPropagation(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    handle()->stopPropagation();
    info.GetReturnValue().SetUndefined();
  }
}
