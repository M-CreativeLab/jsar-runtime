#include "./xr_view.hpp"
#include "./xr_rigid_transform.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace endor
{
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
        InstanceReadonlyAccessor(isolate, instance, "isFirstPersonObserver", &XRView::IsFirstPersonObserverGetter);
        InstanceReadonlyAccessor(isolate, instance, "projectionMatrix", &XRView::ProjectionMatrixGetter);
        InstanceReadonlyAccessor(isolate, instance, "transform", &XRView::TransformGetter);

        // Add methods
        InstanceMethod(isolate, prototype, "requestViewportScale", &XRView::RequestViewportScale);
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

      void XRView::IsFirstPersonObserverGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        // For simplicity, we assume views are first-person observers
        info.GetReturnValue().Set(Boolean::New(isolate, true));
      }

      void XRView::ProjectionMatrixGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        const glm::mat4 &matrix = handle()->projectionMatrix();
        constexpr int len = 16;

        Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, len * sizeof(float));
        float *data = static_cast<float *>(buffer->GetBackingStore()->Data());
        memcpy(data, &matrix, len * sizeof(float));
        info.GetReturnValue().Set(Float32Array::New(buffer, 0, len));
      }

      void XRView::TransformGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(XRRigidTransform::NewInstance(isolate,
                                                                handle()->transform()));
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
} // namespace endor
