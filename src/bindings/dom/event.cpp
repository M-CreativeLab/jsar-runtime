#include "./event.hpp"

namespace dombinding
{
  using namespace std;
  using namespace Napi;

  void InitEvents(Napi::Env env)
  {
    Event::Init(env);
  }

  // static
  Object Event::Make(Napi::Env env, std::shared_ptr<dom::Event> nativeEvent)
  {
    EscapableHandleScope scope(env);
    Object instance;

    switch (nativeEvent->constructor())
    {
    case dom::DOMEventConstructorType::kEvent:
    default:
      instance = Event::NewInstance(env, nativeEvent);
      break;
    }
    return scope.Escape(instance).ToObject();
  }

  // static
  Napi::Object Event::NewInstance(Napi::Env env, std::shared_ptr<dom::Event> nativeEvent)
  {
    return EventWrap<Event, dom::Event>::NewInstance(env, nativeEvent);
  }

  thread_local Napi::FunctionReference *Event::constructor;

  // static
  Napi::Object Event::Init(Napi::Env env)
  {
    EscapableHandleScope scope(env);
    Function func = DefineClass(env, "Event", GetClassProperties(env));
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    return scope.Escape(func).ToObject();
  }

  Event::Event(const Napi::CallbackInfo &info)
      : EventWrap<Event, dom::Event>(info)
  {
  }
}
