#include "./xr_view.hpp"
#include "./xr_rigid_transform.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    void XRView::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "eye", &XRView::EyeGetter);
      InstanceReadonlyAccessor(isolate, instance, "projectionMatrix", &XRView::ProjectionMatrixGetter);
      InstanceReadonlyAccessor(isolate, instance, "transform", &XRView::TransformGetter);

      // Add methods
      InstanceMethod(isolate, prototype, "requestViewportScale", &XRView::RequestViewportScale);
    }

    XRView::XRView(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRViewBase(isolate, args, true)
    {
    }

    // Property getters

    void XRView::EyeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      string eyeStr;
      switch (handle()->eye())
      {
      case client_xr::XREye::kLeft:
        eyeStr = "left";
        break;
      case client_xr::XREye::kRight:
        eyeStr = "right";
        break;
      case client_xr::XREye::kNone:
      default:
        eyeStr = "none";
        break;
      }

      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    eyeStr.c_str())
                                  .ToLocalChecked());
    }

    void XRView::ProjectionMatrixGetter(const PropertyCallbackInfo<Value> &info)
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

    void XRView::TransformGetter(const PropertyCallbackInfo<Value> &info)
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

    void XRView::RequestViewportScale(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1 || !info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "requestViewportScale", "requires a number argument")));
        return;
      }

      float scale = info[0]->NumberValue(context).FromMaybe(1.0f);
      handle()->requestViewportScale(scale);
      info.GetReturnValue().SetUndefined();
    }
  }
}
