#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_input_sources.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    // Forward declarations
    class XRSpace;
    class XRHand;
    class XRInputSource;
    using XRInputSourceBase = scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>;

    /**
     * XRInputSource wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRInputSource objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRInputSource interface for representing
     * input devices like controllers and hands.
     */
    class XRInputSource : public XRInputSourceBase
    {
    public:
      static std::string Name()
      {
        return "XRInputSource";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      XRInputSource(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void GamepadGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void GripSpaceGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HandGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HandednessGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TargetRayModeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TargetRaySpaceGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };

    /**
     * XRInputSourceArray wrapper for V8 arrays.
     *
     * This class provides array-like functionality for collections of XRInputSource objects.
     */
    class XRInputSourceArray
    {
    public:
      /**
       * Create a new V8 array for XRInputSource objects.
       */
      static v8::Local<v8::Array> NewInstance(v8::Isolate *isolate, const std::vector<std::shared_ptr<client_xr::XRInputSource>> &inputSources);
    };
  }
}