#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_session.hpp>
#include <client/script_bindings/event_target.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    // Forward declarations
    class XRFrame;
    class XRRenderState;
    class XRInputSourceArray;

    class XRSession;
    using XRSessionBase = scripting_base::ObjectWrap<XRSession,
                                                     client_xr::XRSession,
                                                     script_bindings::EventTarget>;

    class XRSession : public XRSessionBase
    {
      using XRSessionBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRSession";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    private:
      // Property getters and setters
      void InputSourcesGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void RenderStateGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void EnvironmentBlendModeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void EnabledFeaturesGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      void RequestAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info);
      void CancelAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info);
      void UpdateRenderState(const v8::FunctionCallbackInfo<v8::Value> &info);
      void RequestReferenceSpace(const v8::FunctionCallbackInfo<v8::Value> &info);
      void End(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
