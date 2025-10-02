#include "./xr_layer.hpp"
#include "./xr_viewport.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // XRLayer implementation

    void XRLayer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      // XRLayer is a base class with no specific properties or methods
    }

    XRLayer::XRLayer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRLayerBase(isolate, args, true)
    {
    }

    // XRWebGLLayer implementation

    void XRWebGLLayer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "antialias", &XRWebGLLayer::AntialiasGetter);
      InstanceReadonlyAccessor(isolate, instance, "ignoreDepthValues", &XRWebGLLayer::IgnoreDepthValuesGetter);
      InstanceReadonlyAccessor(isolate, instance, "multiviewRequired", &XRWebGLLayer::MultiviewRequiredGetter);
      InstanceReadonlyAccessor(isolate, instance, "framebuffer", &XRWebGLLayer::FramebufferGetter);
      InstanceReadonlyAccessor(isolate, instance, "framebufferWidth", &XRWebGLLayer::FramebufferWidthGetter);
      InstanceReadonlyAccessor(isolate, instance, "framebufferHeight", &XRWebGLLayer::FramebufferHeightGetter);
      InstanceAccessor(isolate,
                       instance,
                       "fixedFoveation",
                       &XRWebGLLayer::FixedFoveationGetter,
                       &XRWebGLLayer::FixedFoveationSetter);

      // Add methods
      InstanceMethod(isolate, instance, "getViewport", &XRWebGLLayer::GetViewport);

      // Add static methods
      tpl->Set(String::NewFromUtf8(isolate, "getNativeFramebufferScaleFactor").ToLocalChecked(),
               FunctionTemplate::New(isolate, GetNativeFramebufferScaleFactor));
    }

    // static
    void XRWebGLLayer::GetNativeFramebufferScaleFactor(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // TODO: Get actual native framebuffer scale factor
      cout << "XRWebGLLayer.getNativeFramebufferScaleFactor called" << endl;
      info.GetReturnValue().Set(Number::New(isolate, 1.0));
    }

    XRWebGLLayer::XRWebGLLayer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRWebGLLayerBase(isolate, args, true)
    {
    }

    // Property getters and setters

    void XRWebGLLayer::AntialiasGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto antialias = handle()->antialias;
      info.GetReturnValue().Set(Boolean::New(isolate, antialias));
    }

    void XRWebGLLayer::IgnoreDepthValuesGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto ignoreDepthValues = handle()->ignoreDepthValues;
      info.GetReturnValue().Set(Boolean::New(isolate, ignoreDepthValues));
    }

    void XRWebGLLayer::MultiviewRequiredGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto multiviewRequired = handle()->multiviewRequired;
      info.GetReturnValue().Set(Boolean::New(isolate, multiviewRequired));
    }

    void XRWebGLLayer::FramebufferGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual WebGL framebuffer from native layer
      cout << "webglLayer.framebuffer getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRWebGLLayer::FramebufferWidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto width = handle()->framebufferWidth;
      info.GetReturnValue().Set(Number::New(isolate, width));
    }

    void XRWebGLLayer::FramebufferHeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto height = handle()->framebufferHeight;
      info.GetReturnValue().Set(Number::New(isolate, height));
    }

    void XRWebGLLayer::FixedFoveationGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Get actual fixed foveation setting from native layer
      info.GetReturnValue().SetNull();
    }

    void XRWebGLLayer::FixedFoveationSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        return;
      }

      // TODO: Set actual fixed foveation setting on native layer
      cout << "webglLayer.fixedFoveation setter called" << endl;
    }

    // Methods

    void XRWebGLLayer::GetViewport(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual XRViewport for the given view
      cout << "webglLayer.getViewport called" << endl;
      info.GetReturnValue().SetNull();
    }
  }
}
