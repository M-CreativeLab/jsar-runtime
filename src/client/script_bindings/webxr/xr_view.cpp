#include "./xr_view.hpp"
#include "./xr_rigid_transform.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // static
    void XRView::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "eye").ToLocalChecked(),
                                    EyeGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "projectionMatrix").ToLocalChecked(),
                                    ProjectionMatrixGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "transform").ToLocalChecked(),
                                    TransformGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "requestViewportScale").ToLocalChecked(),
                            FunctionTemplate::New(isolate, RequestViewportScale));
    }

    // static
    Local<Object> XRView::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRView> nativeView)
    {
      EscapableHandleScope scope(isolate);

      if (nativeView == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRView, client_xr::XRView>::NewInstance(isolate, nativeView).As<Object>());
    }

    // static
    Local<Function> XRView::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRView, client_xr::XRView>::Initialize(isolate);
    }

    XRView::XRView(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRView, client_xr::XRView>(isolate, args)
    {
    }

    // Property getters

    // static
    void XRView::EyeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRView *view = Unwrap(isolate, info.This());
      if (view == nullptr || view->inner() == nullptr)
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "none").ToLocalChecked());
        return;
      }

      // TODO: Get actual eye from native view (left, right, none)
      cout << "view.eye getter called" << endl;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "left").ToLocalChecked());
    }

    // static
    void XRView::ProjectionMatrixGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRView *view = Unwrap(isolate, info.This());
      if (view == nullptr || view->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual projection matrix as Float32Array from native
      cout << "view.projectionMatrix getter called" << endl;
      Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, 16 * sizeof(float));
      Local<Float32Array> matrix = Float32Array::New(buffer, 0, 16);
      info.GetReturnValue().Set(matrix);
    }

    // static
    void XRView::TransformGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRView *view = Unwrap(isolate, info.This());
      if (view == nullptr || view->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return XRRigidTransform for the view's transform
      cout << "view.transform getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // Methods

    // static
    void XRView::RequestViewportScale(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRView *view = Unwrap(isolate, info.This());
      if (view == nullptr || view->inner() == nullptr)
      {
        info.GetReturnValue().SetUndefined();
        return;
      }

      // TODO: Implement viewport scaling request
      cout << "view.requestViewportScale called" << endl;
      info.GetReturnValue().SetUndefined();
    }
  }
}