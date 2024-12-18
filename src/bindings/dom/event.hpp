#pragma once

#include <memory>
#include <napi.h>
#include <client/dom/dom_event_target.hpp>
#include <common/utility.hpp>

namespace dombinding
{
  /**
   * Initialize the event bindings.
   *
   * @param env The N-API environment.
   */
  void InitEvents(Napi::Env env);

  template <typename ObjectType, typename HandleType>
  class EventWrap : public Napi::ObjectWrap<ObjectType>
  {
    using HandleReference = SharedReference<HandleType>;

  protected:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties()
    {
      using T = EventWrap<ObjectType, HandleType>;
      return {
          T::InstanceAccessor("type", &T::TypeGetter, nullptr),
          T::InstanceAccessor("bubbles", &T::BubblesGetter, nullptr),
          T::InstanceAccessor("cancelable", &T::CancelableGetter, nullptr),
      };
    }
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle)
    {
      Napi::EscapableHandleScope scope(env);
      SharedReference<HandleType> handleRef(handle);
      auto handleExternal = Napi::External<HandleReference>::New(env, &handleRef);

      Napi::Object instance = ObjectType::constructor->New({handleExternal});
      return scope.Escape(instance).ToObject();
    }

  protected:
    EventWrap(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<ObjectType>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 1)
      {
        auto msg = "Failed to construct 'Event': "
                   "1 argument required, but only " +
                   std::to_string(info.Length()) + " present.";
        Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
        return;
      }

      if (info[0].IsExternal())
      {
        auto handleExternal = info[0].As<Napi::External<HandleReference>>();
        auto handleRef = handleExternal.Data();
        if (!handleRef)
        {
          Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
          return;
        }
        handle_ = handleRef->value;
      }
    }

  private:
    Napi::Value TypeGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return Napi::String::New(env, handle_->typeStr());
    }
    Napi::Value BubblesGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return Napi::Boolean::New(env, handle_->bubbles());
    }
    Napi::Value CancelableGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return Napi::Boolean::New(env, handle_->cancelable());
    }

  protected:
    std::shared_ptr<HandleType> handle_;
  };

  /**
   * The `Event` class represents an event object.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/API/Event
   */
  class Event : public EventWrap<Event, dom::Event>
  {
    friend class EventWrap<Event, dom::Event>;

  public:
    /**
     * Create a new JavaScript object that inherits from `Event` class and wraps the native `dom::Event` object.
     *
     * @param env The N-API environment.
     * @param nativeEvent The native `dom::Event` object to wrap.
     * @returns The JavaScript object that wraps the native `dom::Event` object.
     */
    static Napi::Object Make(Napi::Env env, std::shared_ptr<dom::Event> nativeEvent);
    /**
     * Create a new `Event` object.
     *
     * @param env The N-API environment.
     * @param nativeEvent The native `dom::Event` object to wrap.
     * @returns The JavaScript object that wraps the native `dom::Event` object.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<dom::Event> nativeEvent);
    /**
     * Initialize the `Event` class.
     */
    static Napi::Object Init(Napi::Env env);

  public:
    Event(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
