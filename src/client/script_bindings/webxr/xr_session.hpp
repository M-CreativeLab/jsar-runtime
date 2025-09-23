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
    public:
      /**
       * The name of the XRSession class for V8.
       */
      static std::string Name()
      {
        return "XRSession";
      }

      /**
       * Configure the V8 function template with XRSession properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRSession instance from a native client_xr::XRSession.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRSession> nativeSession);

      /**
       * Initialize the XRSession class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRSession(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      static void InputSourcesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void RenderStateGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void EnvironmentBlendModeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void EnabledFeaturesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void RequestAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CancelAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void UpdateRenderState(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void RequestReferenceSpace(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void End(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}