#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_renderstate.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    // Forward declarations
    class XRLayer;

    /**
     * XRRenderState wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRRenderState objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRRenderState interface for managing
     * rendering state including layers and depth settings.
     */
    class XRRenderState : public scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>
    {
    public:
      /**
       * The name of the XRRenderState class for V8.
       */
      static std::string Name()
      {
        return "XRRenderState";
      }

      /**
       * Configure the V8 function template with XRRenderState properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRRenderState instance from a native client_xr::XRRenderState.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRRenderState> nativeRenderState);

      /**
       * Initialize the XRRenderState class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRRenderState(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void BaseLayerGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DepthFarGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DepthNearGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void InlineVerticalFieldOfViewGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}