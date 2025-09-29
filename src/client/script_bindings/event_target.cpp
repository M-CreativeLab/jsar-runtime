#include <iostream>
#include "./event_target.hpp"
#include "./event.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  // static
  void EventTarget::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    // Add methods
    InstanceMethod(isolate, prototype, "addEventListener", &EventTarget::AddEventListener);
    InstanceMethod(isolate, prototype, "removeEventListener", &EventTarget::RemoveEventListener);
    InstanceMethod(isolate, prototype, "dispatchEvent", &EventTarget::DispatchEvent);
  }

  EventTarget::EventTarget(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : EventTargetBase(isolate, args)
  {
  }

  void EventTarget::AddEventListener(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "addEventListener", "addEventListener requires at least 2 arguments")));
      return;
    }

    if (!info[0]->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "addEventListener", "First argument must be a string")));
      return;
    }

    if (!info[1]->IsFunction())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "addEventListener", "Second argument must be a function")));
      return;
    }

    String::Utf8Value typeString(isolate, info[0]);
    Local<Function> listener = Local<Function>::Cast(info[1]);

    auto listenerCallback = [](::dom::DOMEventType type, shared_ptr<::dom::Event> event)
    {
      cout << "Event triggered: " << static_cast<int>(type) << endl;
    };

    std::optional<dom::DOMEventType> eventType = dom::StringToEventType(*typeString, eventTargetType());
    auto listenerHandle = handle()->addEventListener(*eventType, listenerCallback);
    info.GetReturnValue().SetUndefined();
  }

  void EventTarget::RemoveEventListener(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "removeEventListener requires at least 2 arguments").ToLocalChecked()));
      return;
    }

    EventTarget *eventTarget = Unwrap(isolate, info.This());
    if (eventTarget == nullptr || eventTarget->inner() == nullptr)
    {
      return;
    }

    String::Utf8Value eventType(isolate, info[0]);
    // TODO: Implement actual event listener removal
    cout << "removeEventListener called for event: " << *eventType << endl;
  }

  void EventTarget::DispatchEvent(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "dispatchEvent", "dispatchEvent requires 1 argument")));
      return;
    }

    if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "dispatchEvent", "First argument must be an Event object")));
      return;
    }

    Event* event = Event::Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
    if (event == nullptr || event->inner() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "dispatchEvent", "Invalid Event object")));
      return;
    }

    bool success = handle()->dispatchEvent(event->handle());
    info.GetReturnValue().Set(Boolean::New(isolate, success));
  }
}
