#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_frame.hpp>

namespace script_bindings
{
  namespace webxr
  {
    // Forward declarations
    class XRSession;
    class XRPose;
    class XRViewerPose;
    class XRSpace;

    /**
     * XRFrame wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRFrame objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRFrame interface for accessing pose data,
     * hit testing, and anchor creation within an animation frame.
     */
    class XRFrame : public scripting_base::ObjectWrap<XRFrame, client_xr::XRFrame>
    {
    public:
      /**
       * The name of the XRFrame class for V8.
       */
      static std::string Name()
      {
        return "XRFrame";
      }

      /**
       * Configure the V8 function template with XRFrame properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRFrame instance from a native client_xr::XRFrame.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRFrame> nativeFrame);

      /**
       * Initialize the XRFrame class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRFrame(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void SessionGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void GetPose(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetViewerPose(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CreateAnchor(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetHitTestResults(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetHitTestResultsForTransientInput(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void FillPoses(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void FillJointRadii(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}