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

  protected:
    static Napi::Value OnEventGlobalListenerCallback(const Napi::CallbackInfo &info);
    static Napi::Value OnEventListenerCallback(const Napi::CallbackInfo &info);

  public:
    EventTargetWrap(const Napi::CallbackInfo &info);
    virtual ~EventTargetWrap();

  protected:
    Napi::Value AddEventListener(const Napi::CallbackInfo &info);
    Napi::Value RemoveEventListener(const Napi::CallbackInfo &info);
    Napi::Value DispatchEvent(const Napi::CallbackInfo &info);

  protected:
    /**
     * Set the native event target object to initialize this class.
     * 
     * @param eventTarget The native event target object.
     */
    void setEventTarget(std::shared_ptr<EventTargetType> eventTarget);

  protected:
    std::shared_ptr<EventTargetType> eventTarget = nullptr;

  private:
    std::thread::id jsThreadId;
    std::unordered_map<std::shared_ptr<Napi::FunctionReference>, uint32_t> listenerRefToNativeIdMap;
    Napi::FunctionReference globalListenerCallback;
    Napi::FunctionReference listenerCallback;
    /**
     * The N-API thread-safe functions to be called when the dispatcher fires from other threads.
     */
    Napi::ThreadSafeFunction threadSafeGlobalListenerCallback;
    Napi::ThreadSafeFunction threadSafeListenerCallback;
  };
}
