#pragma once

#include <napi.h>
#include "./event_target.hpp"
#include "./event.hpp"

namespace dombinding
{
  /**
   * The callback function is used to call `on${eventType}` property of the event target object.
   */
  template <typename ObjectType, typename EventTargetType>
  Napi::Value EventTargetWrap<ObjectType, EventTargetType>::OnEventGlobalListenerCallback(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      auto msg = "Failed to dispatch event on 'EventTarget': "
                 "1 argument required, but only " +
                 to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsObject())
    {
      auto msg = "Failed to dispatch event on 'EventTarget': "
                 "The event object provided is not an object.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info.This().IsObject())
    {
      auto msg = "Failed to dispatch event on 'EventTarget': "
                 "The 'this' object is not an object.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsEvent = info[0].ToObject();
    auto jsEventType = jsEvent.Get("type").ToString();
    auto jsThis = info.This().As<Napi::Object>();

    auto onEventName = "on" + ToLowerCase(jsEventType.Utf8Value());
    if (jsThis.Has(onEventName))
    {
      auto onEventValue = jsThis.Get(onEventName);
      if (onEventValue.IsFunction())
        onEventValue.As<Napi::Function>().Call(jsThis, {jsEvent});
    }
    return env.Undefined();
  }

  /**
   * The callback function is used to call the event listener function, registered by `addEventListener` method.
   */
  template <typename ObjectType, typename EventTargetType>
  Napi::Value EventTargetWrap<ObjectType, EventTargetType>::OnEventListenerCallback(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      auto msg = "Failed to dispatch event on 'EventTarget': "
                 "2 argument required, but only " +
                 to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsFunction())
    {
      auto msg = "Failed to dispatch event on 'EventTarget': "
                 "The listener provided is not a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    info[1].As<Napi::Function>().Call(info.This(), {info[0]});
    return env.Undefined();
  }

  template <typename ObjectType, typename EventTargetType>
  EventTargetWrap<ObjectType, EventTargetType>::EventTargetWrap(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ObjectType>(info),
        jsThreadId(std::this_thread::get_id())
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    {
      // Initialize callbacks for global event listener.
      Napi::Function callbackFn = Napi::Function::New(env, &EventTargetWrap<ObjectType, EventTargetType>::OnEventGlobalListenerCallback);
      globalListenerCallback = Napi::Persistent(callbackFn);

      Napi::Object callbackObject = callbackFn.ToObject();
      Napi::Function onEventGlobalListenerCallback = callbackObject
                                                         .Get("bind")
                                                         .As<Napi::Function>()
                                                         .Call(callbackObject, {info.This()})
                                                         .As<Napi::Function>();
      threadSafeGlobalListenerCallback = Napi::ThreadSafeFunction::New(env, onEventGlobalListenerCallback, "EventGlobalListenerCallback", 0, 2);
    }

    {
      // Initialize callbacks for event listener.
      Napi::Function callbackFn = Napi::Function::New(env, &EventTargetWrap<ObjectType, EventTargetType>::OnEventListenerCallback);
      listenerCallback = Napi::Persistent(callbackFn);

      Napi::Object callbackObject = callbackFn.ToObject();
      Napi::Function onEventListenerCallback = callbackObject
                                                   .Get("bind")
                                                   .As<Napi::Function>()
                                                   .Call(callbackObject, {info.This()})
                                                   .As<Napi::Function>();
      threadSafeListenerCallback = Napi::ThreadSafeFunction::New(env, onEventListenerCallback, "EventListenerCallback", 0, 2);
    }
  }

  template <typename ObjectType, typename EventTargetType>
  EventTargetWrap<ObjectType, EventTargetType>::~EventTargetWrap()
  {
    globalListenerCallback.Reset();
    listenerCallback.Reset();
    threadSafeGlobalListenerCallback.Release();
    threadSafeListenerCallback.Release();
    if (eventTarget != nullptr)
      eventTarget->resetGlobalEventListener();
  }

  template <typename ObjectType, typename EventTargetType>
  Napi::Value EventTargetWrap<ObjectType, EventTargetType>::AddEventListener(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (eventTarget == nullptr)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "The event target object is not initialized.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 2)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "2 arguments required, but only " +
                 to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto typeString = info[0].ToString().Utf8Value();
    auto listenerValue = info[1];

    if (!listenerValue.IsFunction())
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': "
                 "The listener provided is not a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    dom::DOMEventType eventType;
    try
    {
      eventType = dom::StringToEventType(typeString, jsConstructorName);
    }
    catch (const invalid_argument &e)
    {
      auto msg = "Failed to execute 'addEventListener' on 'EventTarget': " + string(e.what());
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

#define PRINT_LISTENER_ERROR(type, err) \
  std::cerr << "occurred an error when calling listener on event(" << type << "): " << err.Message() << std::endl

    auto jsThisRef = make_shared<Napi::ObjectReference>(Napi::Persistent(info.This().As<Napi::Object>()));
    auto listenerRef = make_shared<Napi::FunctionReference>(Napi::Persistent(listenerValue.As<Napi::Function>()));
    auto listenerCallback = [this, jsThisRef, listenerRef](dom::DOMEventType type, std::shared_ptr<dom::Event> event)
    {
      if (this->jsThreadId != std::this_thread::get_id())
      {
        /**
         * When the caller thread is not the same as the JavaScript thread, we need to call tsfn.
         */
        threadSafeListenerCallback.NonBlockingCall(
            [type, event, listenerRef](Napi::Env env, Napi::Function jsCallback)
            {
              Napi::HandleScope scope(env);
              try
              {
                auto jsEvent = dombinding::Event::Make(env, event);
                jsCallback.Call({jsEvent, listenerRef->Value()});
              }
              catch (const Napi::Error &e)
              {
                std::string eventTypeStr = dom::EventTypeToString(type);
                PRINT_LISTENER_ERROR(eventTypeStr, e);
              }
            });
        return;
      }
      else
      {
        /**
         * When the caller thread is the same as the JavaScript thread, we can call the listener directly.
         */
        Napi::Env env = jsThisRef->Env();
        Napi::HandleScope scope(env);

        assert(!this->listenerCallback.IsEmpty());
        try
        {
          auto jsEvent = dombinding::Event::Make(env, event);
          this->listenerCallback.Call(jsThisRef->Value(), {jsEvent, listenerRef->Value()});
        }
        catch (const Napi::Error &e)
        {
          std::string eventTypeStr = dom::EventTypeToString(type);
          PRINT_LISTENER_ERROR(eventTypeStr, e);
        }
      }
    };

    auto nativeListener = eventTarget->addEventListener(eventType, listenerCallback);
    listenerRefToNativeIdMap.insert({listenerRef, nativeListener->id});
    return env.Undefined();
  }

  template <typename ObjectType, typename EventTargetType>
  Napi::Value EventTargetWrap<ObjectType, EventTargetType>::RemoveEventListener(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (eventTarget == nullptr)
    {
      auto msg = "Failed to execute 'removeEventListener' on 'EventTarget': "
                 "The event target object is not initialized.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 2)
    {
      auto msg = "Failed to execute 'removeEventListener' on 'EventTarget': "
                 "2 arguments required, but only " +
                 to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto typeString = info[0].ToString().Utf8Value();
    auto listenerValue = info[1];

    if (!listenerValue.IsFunction())
    {
      auto msg = "Failed to execute 'removeEventListener' on 'EventTarget': "
                 "The listener provided is not a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    dom::DOMEventType eventType;
    try
    {
      eventType = dom::StringToEventType(typeString, jsConstructorName);
    }
    catch (const invalid_argument &e)
    {
      auto msg = "Failed to execute 'removeEventListener' on 'EventTarget': " + string(e.what());
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    for (auto it = listenerRefToNativeIdMap.begin(); it != listenerRefToNativeIdMap.end();)
    {
      auto listenerRef = it->first;
      if (listenerRef->Value() == listenerValue)
      {
        uint32_t listenerId = it->second;
        eventTarget->removeEventListener(eventType, listenerId);
        it = listenerRefToNativeIdMap.erase(it);
        break;
      }
      else
      {
        ++it;
      }
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename EventTargetType>
  Napi::Value EventTargetWrap<ObjectType, EventTargetType>::DispatchEvent(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (eventTarget == nullptr)
    {
      auto msg = "Failed to execute 'dispatchEvent' on 'EventTarget': "
                 "The event target object is not initialized.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 1 || !info[0].IsObject())
    {
      auto msg = "Failed to execute 'dispatchEvent' on 'EventTarget': "
                 "1 argument required, but only 0 present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto eventObject = info[0].ToObject();
    if (!eventObject.InstanceOf(env.Global().Get("Event").As<Napi::Function>()))
    {
      auto msg = "Failed to execute 'dispatchEvent' on 'EventTarget': "
                 "parameter 1 is not of type 'Event'.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto eventTypeString = eventObject.Get("type").ToString().Utf8Value();
    dom::DOMEventType eventType;
    try
    {
      eventType = dom::StringToEventType(eventTypeString, jsConstructorName);
    }
    catch (const invalid_argument &e)
    {
      auto msg = "Failed to execute 'dispatchEvent' on 'EventTarget': " + string(e.what());
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    eventTarget->dispatchEvent(eventType, nullptr);
    return Napi::Boolean::New(env, true);
  }

  template <typename ObjectType, typename EventTargetType>
  void EventTargetWrap<ObjectType, EventTargetType>::setEventTarget(std::shared_ptr<EventTargetType> eventTarget,
                                                                    std::optional<std::string> jsConstructorName)
  {
    auto listenerCallback = [this](dom::DOMEventType type, std::shared_ptr<dom::Event> event)
    {
      if (this->jsThreadId != std::this_thread::get_id())
      {
        this->threadSafeGlobalListenerCallback.NonBlockingCall(
            [event](Napi::Env env, Napi::Function jsCallback)
            {
              Napi::HandleScope scope(env);
              auto jsEvent = dombinding::Event::Make(env, event);
              jsCallback.Call({jsEvent});
            });
        return;
      }
      else
      {
        assert(!this->globalListenerCallback.IsEmpty());
        Napi::Env env = this->globalListenerCallback.Env();
        Napi::HandleScope scope(env);
        auto jsEvent = dombinding::Event::Make(env, event);
        this->globalListenerCallback.Call(this->Value(), {jsEvent});
      }
    };

    assert(this->eventTarget == nullptr);
    this->eventTarget = eventTarget;
    this->eventTarget->resetGlobalEventListener(listenerCallback);

    if (jsConstructorName.has_value())
      this->jsConstructorName = jsConstructorName;
  }
}
