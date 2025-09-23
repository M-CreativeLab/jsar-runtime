#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_poses.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    // Forward declarations
    class XRRigidTransform;

    /**
     * XRPose wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRPose objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRPose interface for representing
     * position and orientation at a specific time.
     */
    class XRPose : public scripting_base::ObjectWrap<XRPose, client_xr::XRPose>
    {
    public:
      /**
       * The name of the XRPose class for V8.
       */
      static std::string Name()
      {
        return "XRPose";
      }

      /**
       * Configure the V8 function template with XRPose properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRPose instance from a native client_xr::XRPose.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRPose> nativePose);

      /**
       * Initialize the XRPose class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRPose(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void TransformGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void EmulatedPositionGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };

    /**
     * XRViewerPose wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRViewerPose objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRViewerPose interface for representing
     * the viewer's pose with associated views.
     */
    class XRViewerPose : public scripting_base::ObjectWrap<XRViewerPose, client_xr::XRViewerPose, XRPose>
    {
    public:
      /**
       * The name of the XRViewerPose class for V8.
       */
      static std::string Name()
      {
        return "XRViewerPose";
      }

      /**
       * Configure the V8 function template with XRViewerPose properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRViewerPose instance from a native client_xr::XRViewerPose.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRViewerPose> nativeViewerPose);

      /**
       * Initialize the XRViewerPose class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRViewerPose(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void ViewsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}