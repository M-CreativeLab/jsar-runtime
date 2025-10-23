#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_rigid_transform.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    class XRRigidTransform;
    using XRRigidTransformBase = scripting_base::ObjectWrap<XRRigidTransform, client_xr::XRRigidTransform>;

    /**
     * XRRigidTransform wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRRigidTransform objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRRigidTransform interface for representing
     * position and orientation in 3D space.
     */
    class XRRigidTransform : public XRRigidTransformBase
    {
      using XRRigidTransformBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRRigidTransform";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, const client_xr::XRRigidTransform &transform);

    public:
      XRRigidTransform(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void PositionGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void OrientationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void MatrixGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void InverseGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}