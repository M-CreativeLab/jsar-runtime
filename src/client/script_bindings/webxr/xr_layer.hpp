#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_layers.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    class XRLayer;
    using XRLayerBase = scripting_base::ObjectWrap<XRLayer, client_xr::XRLayer>;

    /**
     * XRLayer wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRLayer objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRLayer interface for representing
     * base layer functionality.
     */
    class XRLayer : public XRLayerBase
    {
      using XRLayerBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRLayer";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    };

    class XRWebGLLayer;
    using XRWebGLLayerBase = scripting_base::ObjectWrap<XRWebGLLayer, client_xr::XRWebGLLayer, XRLayer>;

    /**
     * XRWebGLLayer wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRWebGLLayer objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRWebGLLayer interface for WebGL rendering
     * with framebuffer management and viewport access.
     */
    class XRWebGLLayer : public XRWebGLLayerBase
    {
      using XRWebGLLayerBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRWebGLLayer";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      XRWebGLLayer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Static methods
      static void GetNativeFramebufferScaleFactor(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Property getters and setters
      void AntialiasGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void IgnoreDepthValuesGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void MultiviewRequiredGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FramebufferGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FramebufferWidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FramebufferHeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FixedFoveationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FixedFoveationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      void GetViewport(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
