#include <bindings/webxr/frame.hpp>
#include <bindings/webxr/input_source.hpp>
#include <bindings/webxr/session.hpp>
#include "./webxr_inputsource.hpp"

namespace dombinding::events
{
  using namespace std;
  using namespace Napi;

  // static
  Object XRInputSourceEvent::NewInstance(Napi::Env env, shared_ptr<client_xr::XRInputSourceEvent> nativeEvent)
  {
    return EventWrap<XRInputSourceEvent, client_xr::XRInputSourceEvent>::NewInstance(env, nativeEvent);
  }

  thread_local FunctionReference *XRInputSourceEvent::constructor;

  // static
  Object XRInputSourceEvent::Init(Napi::Env env)
  {
    EscapableHandleScope scope(env);
    auto props = GetClassProperties(env);
    auto added = {
      InstanceValue("frame", env.Null(), napi_default_jsproperty),
      InstanceValue("inputSource", env.Null(), napi_default_jsproperty),
    };
    props.insert(props.end(), added.begin(), added.end());
    Function func = DefineClass(env, "XRInputSourceEvent", props);
    constructor = new FunctionReference();
    *constructor = Persistent(func);
    return scope.Escape(func).ToObject();
  }

  XRInputSourceEvent::XRInputSourceEvent(const Napi::CallbackInfo &info)
      : EventWrap<XRInputSourceEvent, client_xr::XRInputSourceEvent>(info)
  {
    auto env = info.Env();
    HandleScope scope(env);

    auto jsThis = info.This().As<Object>();
    auto currentFrame = handle_->frame();
    if (currentFrame != nullptr)
    {
      auto session = currentFrame->session();
      assert(session != nullptr && "session must be existed when constructing XRInputSourceEvent");
      jsThis.Set("frame", bindings::XRFrame::GetOrNewInstance(env, &session->getJSObject(), currentFrame));
    }

    auto currentInputSource = handle_->inputSource();
    if (currentInputSource != nullptr && currentInputSource->isJSObject())
      jsThis.Set("inputSource", currentInputSource->getJSObject().Value());
  }
}
