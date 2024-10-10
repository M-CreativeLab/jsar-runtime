#pragma once

#include <memory>
#include <unordered_map>
#include <thread>
#include <napi.h>
#include "client/dom/dom_event_target.hpp"

namespace dombinding
{
  template <typename ObjectType, typename EventTargetType>
  class EventTargetWrap : public Napi::ObjectWrap<ObjectType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties()
    {
      using T = EventTargetWrap<ObjectType, EventTargetType>;
      return {
          T::InstanceMethod("addEventListener", &T::AddEventListener),
          T::InstanceMethod("removeEventListener", &T::RemoveEventListener),
          T::InstanceMethod("dispatchEvent", &T::DispatchEvent),
      };
    }

  public:
    EventTargetWrap(const Napi::CallbackInfo &info);
    virtual ~EventTargetWrap();

  protected:
    Napi::Value AddEventListener(const Napi::CallbackInfo &info);
    Napi::Value RemoveEventListener(const Napi::CallbackInfo &info);
    Napi::Value DispatchEvent(const Napi::CallbackInfo &info);

  protected:
    static Napi::Value OnEventListenerCallback(const Napi::CallbackInfo &info);

  protected:
    std::shared_ptr<EventTargetType> eventTarget;

  private:
    std::thread::id jsThreadId;
    std::unordered_map<std::shared_ptr<Napi::FunctionReference>, uint32_t> listenerRefToNativeIdMap;
    Napi::FunctionReference listenerCallback;
    /**
     * The N-API thread-safe function to be called when the dispatcher fires from other threads.
     */
    Napi::ThreadSafeFunction threadSafeListenerCallback;
  };
}
