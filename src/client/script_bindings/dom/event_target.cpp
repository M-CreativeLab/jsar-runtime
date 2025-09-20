#include "./event_target.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    // static
    void EventTarget::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "addEventListener").ToLocalChecked(),
                            FunctionTemplate::New(isolate, AddEventListener));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "removeEventListener").ToLocalChecked(),
                            FunctionTemplate::New(isolate, RemoveEventListener));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "dispatchEvent").ToLocalChecked(),
                            FunctionTemplate::New(isolate, DispatchEvent));
    }

    // static
    Local<Function> EventTarget::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<EventTarget, ::dom::EventTarget>::Initialize(isolate);
    }

    EventTarget::EventTarget(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<EventTarget, ::dom::EventTarget>(isolate, args)
    {
    }

    // Event methods

    // static
    void EventTarget::AddEventListener(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "addEventListener requires at least 2 arguments").ToLocalChecked()));
        return;
      }

      if (!info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "First argument must be a string").ToLocalChecked()));
        return;
      }

      if (!info[1]->IsFunction())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Second argument must be a function").ToLocalChecked()));
        return;
      }

      EventTarget *eventTarget = scripting_base::ObjectWrap<EventTarget, ::dom::EventTarget>::Unwrap(info.This());
      if (eventTarget == nullptr || eventTarget->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value eventType(isolate, info[0]);
      Local<Function> listener = Local<Function>::Cast(info[1]);

      // For now, store the listener. In a full implementation, this would register
      // with the DOM event system
      // TODO: Implement actual event listener registration with the DOM backend
      cout << "addEventListener called for event: " << *eventType << endl;
    }

    // static
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

      EventTarget *eventTarget = scripting_base::ObjectWrap<EventTarget, ::dom::EventTarget>::Unwrap(info.This());
      if (eventTarget == nullptr || eventTarget->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value eventType(isolate, info[0]);
      // TODO: Implement actual event listener removal
      cout << "removeEventListener called for event: " << *eventType << endl;
    }

    // static
    void EventTarget::DispatchEvent(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "dispatchEvent requires 1 argument").ToLocalChecked()));
        return;
      }

      if (!info[0]->IsObject())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "First argument must be an Event object").ToLocalChecked()));
        return;
      }

      EventTarget *eventTarget = scripting_base::ObjectWrap<EventTarget, ::dom::EventTarget>::Unwrap(info.This());
      if (eventTarget == nullptr || eventTarget->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement actual event dispatching
      cout << "dispatchEvent called" << endl;
      info.GetReturnValue().Set(Boolean::New(isolate, true));
    }
  }
}