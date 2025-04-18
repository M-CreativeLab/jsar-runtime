#include <bindings/webxr/session.hpp>
#include "./webxr_session.hpp"

namespace dombinding::events
{
  using namespace std;
  using namespace Napi;

  // static
  Object XRSessionEvent::NewInstance(Napi::Env env, shared_ptr<client_xr::XRSessionEvent> nativeEvent)
  {
    return EventWrap<XRSessionEvent, client_xr::XRSessionEvent>::NewInstance(env, nativeEvent);
  }

  thread_local FunctionReference *XRSessionEvent::constructor;

  // static
  Object XRSessionEvent::Init(Napi::Env env)
  {
    EscapableHandleScope scope(env);
    auto props = GetClassProperties(env);
    auto added = {
        InstanceValue("session", env.Null(), napi_default_jsproperty),
    };
    props.insert(props.end(), added.begin(), added.end());
    Function func = DefineClass(env, "XRSessionEvent", props);
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    return scope.Escape(func).ToObject();
  }

  XRSessionEvent::XRSessionEvent(const Napi::CallbackInfo &info)
      : EventWrap<XRSessionEvent, client_xr::XRSessionEvent>(info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    auto jsThis = info.This().As<Object>();
    auto session = bindings::XRSession::NewInstance(env, handle_->session());
    jsThis.Set("session", session);
  }
}
