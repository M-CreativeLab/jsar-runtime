#include "network_event.hpp"

namespace dombinding
{
  namespace events
  {
    thread_local Napi::FunctionReference *NetworkEvent::constructor;

    void NetworkEvent::Init(Napi::Env env)
    {
      Napi::EscapableHandleScope scope(env);
      Napi::Function func = DefineClass(env, "NetworkEvent", GetClassProperties(env));
      constructor = new Napi::FunctionReference();
      *constructor = Napi::Persistent(func);
      env.Global().Set("NetworkEvent", func);
    }

    Napi::Object NetworkEvent::NewInstance(Napi::Env env, std::shared_ptr<dom::NetworkEvent> nativeEvent)
    {
      return EventWrap<NetworkEvent, dom::NetworkEvent>::NewInstance(env, nativeEvent);
    }

    NetworkEvent::NetworkEvent(const Napi::CallbackInfo &info)
        : EventWrap<NetworkEvent, dom::NetworkEvent>(info)
    {
    }
  } // namespace events
} // namespace dombinding