#include <bindings/webxr/input_source.hpp>
#include <bindings/webxr/session.hpp>
#include "./webxr_inputsourceschange.hpp"

namespace dombinding::events
{
  using namespace std;
  using namespace Napi;

  // static
  Napi::Object XRInputSourcesChangeEvent::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRInputSourcesChangeEvent> nativeEvent)
  {
    return EventWrap<XRInputSourcesChangeEvent, client_xr::XRInputSourcesChangeEvent>::NewInstance(env, nativeEvent);
  }

  thread_local FunctionReference *XRInputSourcesChangeEvent::constructor;

  // static
  Object XRInputSourcesChangeEvent::Init(Napi::Env env)
  {
    EscapableHandleScope scope(env);
    auto props = GetClassProperties();
    auto added = {
        InstanceValue("session", env.Null(), napi_default_jsproperty),
        InstanceValue("added", env.Null(), napi_default_jsproperty),
        InstanceValue("removed", env.Null(), napi_default_jsproperty),
    };
    props.insert(props.end(), added.begin(), added.end());
    Function func = DefineClass(env, "XRInputSourcesChangeEvent", props);
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    return scope.Escape(func).ToObject();
  }

  XRInputSourcesChangeEvent::XRInputSourcesChangeEvent(const Napi::CallbackInfo &info)
      : EventWrap<XRInputSourcesChangeEvent, client_xr::XRInputSourcesChangeEvent>(info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    auto jsThis = info.This().As<Object>();
    auto session = handle_->session()->getReference();
    jsThis.Set("session", session == nullptr ? env.Null() : session->Value());

    Array added = Array::New(env);
    {
      uint32_t index = 0;
      for (auto &inputSource : handle_->added())
        added.Set(index++, bindings::XRInputSource::NewInstance(env, inputSource));
      jsThis.Set("added", added);
    }

    Array removed = Array::New(env);
    {
      uint32_t index = 0;
      for (auto &inputSource : handle_->removed())
        removed.Set(index++, bindings::XRInputSource::NewInstance(env, inputSource));
      jsThis.Set("removed", removed);
    }
  }
}
