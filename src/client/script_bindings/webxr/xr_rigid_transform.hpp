#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_rigid_transform.hpp>

namespace script_bindings
{
  namespace webxr
  {
    /**
     * XRRigidTransform wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRRigidTransform objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRRigidTransform interface for representing
     * position and orientation in 3D space.
     */
    class XRRigidTransform : public scripting_base::ObjectWrap<XRRigidTransform, client_xr::XRRigidTransform>
    {
    public:
      /**
       * The name of the XRRigidTransform class for V8.
       */
      static std::string Name()
      {
        return "XRRigidTransform";
      }

      /**
       * Configure the V8 function template with XRRigidTransform properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRRigidTransform instance from a native client_xr::XRRigidTransform.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRRigidTransform> nativeTransform);

      /**
       * Initialize the XRRigidTransform class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRRigidTransform(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void PositionGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void OrientationGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void MatrixGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void InverseGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}