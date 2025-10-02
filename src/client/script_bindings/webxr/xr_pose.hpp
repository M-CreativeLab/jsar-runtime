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
    class XRPose;
    using XRPoseBase = scripting_base::ObjectWrap<XRPose, client_xr::XRPose>;

    /**
     * XRPose wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRPose objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRPose interface for representing
     * position and orientation at a specific time.
     */
    class XRPose : public XRPoseBase
    {
      using XRPoseBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRPose";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    private:
      // Property getters
      void TransformGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void EmulatedPositionGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };

    class XRViewerPose;
    using XRViewerPoseBase = scripting_base::ObjectWrap<XRViewerPose, client_xr::XRViewerPose, XRPose>;

    /**
     * XRViewerPose wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRViewerPose objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRViewerPose interface for representing
     * the viewer's pose with associated views.
     */
    class XRViewerPose : public XRViewerPoseBase
    {
      using XRViewerPoseBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRViewerPose";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    private:
      // Property getters
      void ViewsGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}
