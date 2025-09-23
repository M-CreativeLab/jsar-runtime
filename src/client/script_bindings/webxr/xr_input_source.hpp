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

    /**
     * XRInputSource wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRInputSource objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRInputSource interface for representing
     * input devices like controllers and hands.
     */
    class XRInputSource : public scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>
    {
    public:
      /**
       * The name of the XRInputSource class for V8.
       */
      static std::string Name()
      {
        return "XRInputSource";
      }

      /**
       * Configure the V8 function template with XRInputSource properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRInputSource instance from a native client_xr::XRInputSource.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRInputSource> nativeInputSource);

      /**
       * Get or create a V8 XRInputSource instance from a native client_xr::XRInputSource.
       */
      static v8::Local<v8::Object> GetOrNewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRInputSource> nativeInputSource);

      /**
       * Initialize the XRInputSource class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRInputSource(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void GamepadGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void GripSpaceGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HandGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HandednessGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TargetRayModeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TargetRaySpaceGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
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