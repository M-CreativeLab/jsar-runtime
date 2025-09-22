#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_layers.hpp>

namespace script_bindings
{
  namespace webxr
  {
    /**
     * XRLayer wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRLayer objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRLayer interface for representing
     * base layer functionality.
     */
    class XRLayer : public scripting_base::ObjectWrap<XRLayer, client_xr::XRLayer>
    {
    public:
      /**
       * The name of the XRLayer class for V8.
       */
      static std::string Name()
      {
        return "XRLayer";
      }

      /**
       * Configure the V8 function template with XRLayer properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRLayer instance from a native client_xr::XRLayer.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRLayer> nativeLayer);

      /**
       * Initialize the XRLayer class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRLayer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

    /**
     * XRWebGLLayer wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRWebGLLayer objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRWebGLLayer interface for WebGL rendering
     * with framebuffer management and viewport access.
     */
    class XRWebGLLayer : public scripting_base::ObjectWrap<XRWebGLLayer, client_xr::XRWebGLLayer, XRLayer>
    {
    public:
      /**
       * The name of the XRWebGLLayer class for V8.
       */
      static std::string Name()
      {
        return "XRWebGLLayer";
      }

      /**
       * Configure the V8 function template with XRWebGLLayer properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRWebGLLayer instance from a native client_xr::XRWebGLLayer.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRWebGLLayer> nativeLayer);

      /**
       * Initialize the XRWebGLLayer class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

      /**
       * Static method to get native framebuffer scale factor.
       */
      static void GetNativeFramebufferScaleFactor(const v8::FunctionCallbackInfo<v8::Value> &info);

    public:
      XRWebGLLayer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      static void AntialiasGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void IgnoreDepthValuesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void MultiviewRequiredGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FramebufferGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FramebufferWidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FramebufferHeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FixedFoveationGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FixedFoveationSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void GetViewport(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}