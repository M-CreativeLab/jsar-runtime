#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_frame.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    // Forward declarations
    class XRSession;
    class XRPose;
    class XRViewerPose;
    class XRSpace;
    class XRFrame;
    using XRFrameBase = scripting_base::ObjectWrap<XRFrame, client_xr::XRFrame>;

    /**
     * XRFrame wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRFrame objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRFrame interface for accessing pose data,
     * hit testing, and anchor creation within an animation frame.
     */
    class XRFrame : public XRFrameBase
    {
    public:
      static std::string Name()
      {
        return "XRFrame";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRFrame> nativeFrame);

    public:
      XRFrame(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void SessionGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      void GetPose(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetViewerPose(const v8::FunctionCallbackInfo<v8::Value> &info);
      void CreateAnchor(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetHitTestResults(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetHitTestResultsForTransientInput(const v8::FunctionCallbackInfo<v8::Value> &info);
      void FillPoses(const v8::FunctionCallbackInfo<v8::Value> &info);
      void FillJointRadii(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}