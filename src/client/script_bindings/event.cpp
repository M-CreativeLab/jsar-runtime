#include "./event.hpp"
#include <iostream>

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
    InstanceReadonlyAccessor(isolate, prototype, "type", &Event::TypeGetter);
    InstanceReadonlyAccessor(isolate, prototype, "bubbles", &Event::BubblesGetter);
    InstanceReadonlyAccessor(isolate, prototype, "cancelable", &Event::CancelableGetter);
    InstanceReadonlyAccessor(isolate, prototype, "composed", &Event::ComposedGetter);

    // Add methods
    InstanceMethod(isolate, prototype, "preventDefault", &Event::PreventDefault);
    InstanceMethod(isolate, prototype, "stopPropagation", &Event::StopPropagation);
  }

  // static
  Local<Object> Event::NewInstance(Isolate *isolate, std::shared_ptr<dom::Event> nativeEvent)
  {
    EscapableHandleScope scope(isolate);
    assert(nativeEvent != nullptr && "nativeEvent must not be null");
    return scope.Escape(EventBase::NewInstance(isolate, nativeEvent).As<Object>());
  }

  Event::Event(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : EventBase(isolate, args)
  {
  }

  // Property getters

  void Event::TypeGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    string type = handle()->typeStr();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, type.c_str()).ToLocalChecked());
  }

  void Event::BubblesGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate, handle()->bubbles()));
  }

  void Event::CancelableGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate, handle()->cancelable()));
  }

  void Event::ComposedGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate, handle()->composed()));
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
