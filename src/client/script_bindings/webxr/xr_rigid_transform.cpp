#include "./xr_rigid_transform.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // static
    void XRRigidTransform::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "position").ToLocalChecked(),
                                    PositionGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "orientation").ToLocalChecked(),
                                    OrientationGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "matrix").ToLocalChecked(),
                                    MatrixGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "inverse").ToLocalChecked(),
                                    InverseGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
    }

    // static
    Local<Object> XRRigidTransform::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRRigidTransform> nativeTransform)
    {
      EscapableHandleScope scope(isolate);

      if (nativeTransform == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRRigidTransform, client_xr::XRRigidTransform>::NewInstance(isolate, nativeTransform).As<Object>());
    }

    // static
    Local<Function> XRRigidTransform::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRRigidTransform, client_xr::XRRigidTransform>::Initialize(isolate);
    }

    XRRigidTransform::XRRigidTransform(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRRigidTransform, client_xr::XRRigidTransform>(isolate, args)
    {
    }

    // Property getters

    // static
    void XRRigidTransform::PositionGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRigidTransform *transform = Unwrap(isolate, info.This());
      if (transform == nullptr || transform->handle() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return DOMPointReadOnly for position
      cout << "position getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRRigidTransform::OrientationGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRigidTransform *transform = Unwrap(isolate, info.This());
      if (transform == nullptr || transform->handle() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return DOMPointReadOnly for orientation (quaternion)
      cout << "orientation getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRRigidTransform::MatrixGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRigidTransform *transform = Unwrap(isolate, info.This());
      if (transform == nullptr || transform->handle() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return Float32Array for 4x4 transformation matrix
      cout << "matrix getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRRigidTransform::InverseGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRigidTransform *transform = Unwrap(isolate, info.This());
      if (transform == nullptr || transform->handle() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return inverse XRRigidTransform
      cout << "inverse getter called" << endl;
      info.GetReturnValue().SetNull();
    }
  }
}