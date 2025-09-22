#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_hand.hpp>

namespace script_bindings
{
  namespace webxr
  {
    /**
     * XRHand wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRHand objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRHand interface for representing
     * hand tracking data with joint information.
     */
    class XRHand : public scripting_base::ObjectWrap<XRHand, client_xr::XRHand>
    {
    public:
      /**
       * The name of the XRHand class for V8.
       */
      static std::string Name()
      {
        return "XRHand";
      }

      /**
       * Configure the V8 function template with XRHand properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRHand instance from a native client_xr::XRHand.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRHand> nativeHand);

      /**
       * Initialize the XRHand class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRHand(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Map-like methods
      static void Entries(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ForEach(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Get(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Keys(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Values(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Iterator support
      static void Iterator(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Property getters
      static void SizeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}