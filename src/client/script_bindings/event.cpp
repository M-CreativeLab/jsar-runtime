#include "./event.hpp"
#include <iostream>

using namespace std;

namespace script_bindings
{
  // static
  void Event::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    v8::HandleScope scope(isolate);

    // Set up the instance template
    v8::Local<v8::ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    // Add property accessors
    instanceTemplate->SetAccessor(
      v8::String::NewFromUtf8(isolate, "type").ToLocalChecked(),
      TypeGetter,
      nullptr,
      v8::Local<v8::Value>(),
      v8::AccessControl::DEFAULT,
      v8::PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(
      v8::String::NewFromUtf8(isolate, "bubbles").ToLocalChecked(),
      BubblesGetter,
      nullptr,
      v8::Local<v8::Value>(),
      v8::AccessControl::DEFAULT,
      v8::PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(
      v8::String::NewFromUtf8(isolate, "cancelable").ToLocalChecked(),
      CancelableGetter,
      nullptr,
      v8::Local<v8::Value>(),
      v8::AccessControl::DEFAULT,
      v8::PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(
      v8::String::NewFromUtf8(isolate, "composed").ToLocalChecked(),
      ComposedGetter,
      nullptr,
      v8::Local<v8::Value>(),
      v8::AccessControl::DEFAULT,
      v8::PropertyAttribute::ReadOnly);

    // Add methods
    instanceTemplate->Set(
      v8::String::NewFromUtf8(isolate, "preventDefault").ToLocalChecked(),
      v8::FunctionTemplate::New(isolate, PreventDefault));

    instanceTemplate->Set(
      v8::String::NewFromUtf8(isolate, "stopPropagation").ToLocalChecked(),
      v8::FunctionTemplate::New(isolate, StopPropagation));
  }

  // static
  v8::Local<v8::Object> Event::NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::Event> nativeEvent)
  {
    v8::EscapableHandleScope scope(isolate);

    // Use the ObjectWrap NewInstance method to create the wrapper
    napi_env napiEnv = nullptr; // For now, we don't have napi_env in this context
    return scope.Escape(scripting_base::ObjectWrap<Event, dom::Event>::NewInstance(napiEnv, nativeEvent).As<v8::Object>());
  }

  // static
  v8::Local<v8::Object> Event::NewInstanceV8(v8::Isolate *isolate, std::shared_ptr<dom::Event> nativeEvent)
  {
    v8::EscapableHandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // Get the constructor function
    v8::Local<v8::Function> constructor = scripting_base::ObjectWrap<Event, dom::Event>::ConstructorFunction(isolate);
    if (constructor.IsEmpty())
    {
      return v8::Local<v8::Object>();
    }

    // Create arguments for the constructor
    std::vector<v8::Local<v8::Value>> args;
    if (nativeEvent != nullptr)
    {
      SharedReference<dom::Event> eventSharedRef(nativeEvent);
      v8::Local<v8::External> eventExternal = v8::External::New(isolate, &eventSharedRef);
      args.push_back(eventExternal);
    }

    // Create the instance
    v8::Local<v8::Object> jsThis = constructor->NewInstance(context, args.size(), args.data()).ToLocalChecked();
    if (jsThis.IsEmpty())
    {
      return v8::Local<v8::Object>();
    }

    // Note: We skip setNapiEnv since we don't have napi_env in this context
    return scope.Escape(jsThis);
  }

  // static
  v8::Local<v8::Function> Event::Initialize(v8::Isolate *isolate)
  {
    return scripting_base::ObjectWrap<Event, dom::Event>::Initialize(isolate);
  }

  Event::Event(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args, std::shared_ptr<dom::Event> nativeEvent)
      : scripting_base::ObjectWrap<Event, dom::Event>(isolate, args, nativeEvent)
  {
  }

  // Property getters

  // static
  void Event::TypeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    Event *event = scripting_base::ObjectWrap<Event, dom::Event>::Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    std::string type = event->inner()->typeStr();
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, type.c_str()).ToLocalChecked());
  }

  // static
  void Event::BubblesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    Event *event = scripting_base::ObjectWrap<Event, dom::Event>::Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    bool bubbles = event->inner()->bubbles();
    info.GetReturnValue().Set(v8::Boolean::New(isolate, bubbles));
  }

  // static
  void Event::CancelableGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    Event *event = scripting_base::ObjectWrap<Event, dom::Event>::Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    bool cancelable = event->inner()->cancelable();
    info.GetReturnValue().Set(v8::Boolean::New(isolate, cancelable));
  }

  // static
  void Event::ComposedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    Event *event = scripting_base::ObjectWrap<Event, dom::Event>::Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    bool composed = event->inner()->composed();
    info.GetReturnValue().Set(v8::Boolean::New(isolate, composed));
  }

  // Methods

  // static
  void Event::PreventDefault(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    Event *event = scripting_base::ObjectWrap<Event, dom::Event>::Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      return;
    }

    event->inner()->preventDefault();
  }

  // static
  void Event::StopPropagation(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    Event *event = scripting_base::ObjectWrap<Event, dom::Event>::Unwrap(info.This());
    if (event == nullptr || event->inner() == nullptr)
    {
      return;
    }

    event->inner()->stopPropagation();
  }
}