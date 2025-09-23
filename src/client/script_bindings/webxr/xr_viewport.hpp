#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_viewport.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    /**
     * XRViewport wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRViewport objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRViewport interface for representing
     * the viewport within which rendering should occur.
     */
    class XRViewport : public scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>
    {
    public:
      /**
       * The name of the XRViewport class for V8.
       */
      static std::string Name()
      {
        return "XRViewport";
      }

      /**
       * Configure the V8 function template with XRViewport properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRViewport instance from a native client_xr::XRViewport.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRViewport> nativeViewport);

      /**
       * Initialize the XRViewport class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRViewport(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void XGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void YGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}