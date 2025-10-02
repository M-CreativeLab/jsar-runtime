#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_view.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    // Forward declarations
    class XRRigidTransform;
    class XRViewport;
    class XRView;
    using XRViewBase = scripting_base::ObjectWrap<XRView, client_xr::XRView>;

    /**
     * XRView wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRView objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRView interface for representing
     * a single view's projection matrix and transform.
     */
    class XRView : public XRViewBase
    {
    public:
      static std::string Name()
      {
        return "XRView";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      XRView(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void EyeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ProjectionMatrixGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TransformGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      void RequestViewportScale(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}