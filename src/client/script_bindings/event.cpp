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
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    // Add property accessors
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "type").ToLocalChecked(),
                                  TypeGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "bubbles").ToLocalChecked(),
                                  BubblesGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "cancelable").ToLocalChecked(),
                                  CancelableGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "composed").ToLocalChecked(),
                                  ComposedGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    // Add methods
    instanceTemplate->Set(String::NewFromUtf8(isolate, "preventDefault").ToLocalChecked(),
                          FunctionTemplate::New(isolate, PreventDefault));

    instanceTemplate->Set(String::NewFromUtf8(isolate, "stopPropagation").ToLocalChecked(),
                          FunctionTemplate::New(isolate, StopPropagation));
  }

  // static
  Local<Object> Event::NewInstance(Isolate *isolate, std::shared_ptr<dom::Event> nativeEvent)
  {
    EscapableHandleScope scope(isolate);

    // Use the ObjectWrap NewInstance method to create the wrapper
    return scope.Escape(EventBase::NewInstance(isolate, nativeEvent).As<Object>());
  }

  Event::Event(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : EventBase(isolate, args)
  {
  }

  // Property getters

  // static
  void Event::TypeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Event *event = Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    string type = event->inner()->typeStr();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, type.c_str()).ToLocalChecked());
  }

  // static
  void Event::BubblesGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Event *event = Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    bool bubbles = event->inner()->bubbles();
    info.GetReturnValue().Set(Boolean::New(isolate, bubbles));
  }

  // static
  void Event::CancelableGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Event *event = Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    bool cancelable = event->inner()->cancelable();
    info.GetReturnValue().Set(Boolean::New(isolate, cancelable));
  }

  // static
  void Event::ComposedGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Event *event = Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    bool composed = event->inner()->composed();
    info.GetReturnValue().Set(Boolean::New(isolate, composed));
  }

  // Methods

  // static
  void Event::PreventDefault(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Event *event = Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      return;
    }

    event->inner()->preventDefault();
  }

  // static
  void Event::StopPropagation(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Event *event = Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      return;
    }

    event->inner()->stopPropagation();
  }
}
