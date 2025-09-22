#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_view.hpp>

namespace script_bindings
{
  namespace webxr
  {
    // Forward declarations
    class XRRigidTransform;
    class XRViewport;

    /**
     * XRView wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRView objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRView interface for representing
     * a single view's projection matrix and transform.
     */
    class XRView : public scripting_base::ObjectWrap<XRView, client_xr::XRView>
    {
    public:
      /**
       * The name of the XRView class for V8.
       */
      static std::string Name()
      {
        return "XRView";
      }

      /**
       * Configure the V8 function template with XRView properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRView instance from a native client_xr::XRView.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRView> nativeView);

      /**
       * Initialize the XRView class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRView(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void EyeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ProjectionMatrixGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TransformGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void RequestViewportScale(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}