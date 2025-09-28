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

    // static
    void XRLayer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      // XRLayer is a base class with no specific properties or methods
    }

    // static
    Local<Object> XRLayer::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRLayer> nativeLayer)
    {
      EscapableHandleScope scope(isolate);

      if (nativeLayer == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRLayer, client_xr::XRLayer>::NewInstance(isolate, nativeLayer).As<Object>());
    }

    // static
    Local<Function> XRLayer::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRLayer, client_xr::XRLayer>::Initialize(isolate);
    }

    XRLayer::XRLayer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRLayer, client_xr::XRLayer>(isolate, args)
    {
    }

    // XRWebGLLayer implementation

    // static
    void XRWebGLLayer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "antialias").ToLocalChecked(),
                                    AntialiasGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "ignoreDepthValues").ToLocalChecked(),
                                    IgnoreDepthValuesGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "multiviewRequired").ToLocalChecked(),
                                    MultiviewRequiredGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "framebuffer").ToLocalChecked(),
                                    FramebufferGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "framebufferWidth").ToLocalChecked(),
                                    FramebufferWidthGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "framebufferHeight").ToLocalChecked(),
                                    FramebufferHeightGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "fixedFoveation").ToLocalChecked(),
                                    FixedFoveationGetter,
                                    FixedFoveationSetter);

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "getViewport").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetViewport));

      // Add static methods
      tpl->Set(String::NewFromUtf8(isolate, "getNativeFramebufferScaleFactor").ToLocalChecked(),
               FunctionTemplate::New(isolate, GetNativeFramebufferScaleFactor));
    }

    // static
    Local<Object> XRWebGLLayer::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRWebGLLayer> nativeLayer)
    {
      EscapableHandleScope scope(isolate);

      if (nativeLayer == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRWebGLLayer, client_xr::XRWebGLLayer, XRLayer>::NewInstance(isolate, nativeLayer).As<Object>());
    }

    // static
    Local<Function> XRWebGLLayer::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRWebGLLayer, client_xr::XRWebGLLayer, XRLayer>::Initialize(isolate);
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
        : scripting_base::ObjectWrap<XRWebGLLayer, client_xr::XRWebGLLayer, XRLayer>(isolate, args)
    {
    }

    // Property getters and setters

    // static
    void XRWebGLLayer::AntialiasGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      // TODO: Get actual antialias setting from native layer
      info.GetReturnValue().Set(Boolean::New(isolate, false));
    }

    // static
    void XRWebGLLayer::IgnoreDepthValuesGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      // TODO: Get actual ignoreDepthValues setting from native layer
      info.GetReturnValue().Set(Boolean::New(isolate, false));
    }

    // static
    void XRWebGLLayer::MultiviewRequiredGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      // TODO: Get actual multiviewRequired setting from native layer
      info.GetReturnValue().Set(Boolean::New(isolate, false));
    }

    // static
    void XRWebGLLayer::FramebufferGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
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

    // static
    void XRWebGLLayer::FramebufferWidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
        return;
      }

      // TODO: Get actual framebuffer width from native layer
      info.GetReturnValue().Set(Number::New(isolate, 1920));
    }

    // static
    void XRWebGLLayer::FramebufferHeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRWebGLLayer *layer = Unwrap(isolate, info.This());
      if (layer == nullptr || layer->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
        return;
      }

      // TODO: Get actual framebuffer height from native layer
      info.GetReturnValue().Set(Number::New(isolate, 1080));
    }

    // static
    void XRWebGLLayer::FixedFoveationGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
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

    // static
    void XRWebGLLayer::FixedFoveationSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
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

    // static
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
