#include <iostream>
#include <client/script_bindings/webgl/framebuffer.hpp>
#include <client/script_bindings/webgl/webgl_rendering_context.hpp>
#include <client/script_bindings/webgl/webgl2_rendering_context.hpp>
#include "./xr_layer.hpp"
#include "./xr_view.hpp"
#include "./xr_viewport.hpp"
#include "./xr_session.hpp"

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

    XRWebGLLayer::XRWebGLLayer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRWebGLLayerBase(isolate, args)
    {
      HandleScope scope(isolate);

      if (args.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate, "Requires at least 2 arguments")));
        return;
      }
      if (!XRSession::IsInstanceOf(isolate, args[0]))
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate, "First argument must be an XRSession object")));
        return;
      }
      if (!webgl_bindings::WebGLRenderingContext::IsInstanceOf(isolate, args[1]) &&
          !webgl_bindings::WebGL2RenderingContext::IsInstanceOf(isolate, args[1]))
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate,
                               "Second argument must be a WebGLRenderingContext or WebGL2RenderingContext object")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      XRSession *session = XRSession::Unwrap(isolate, args[0]->ToObject(context).ToLocalChecked());
      assert(session != nullptr && "XRSession handle is null");

      shared_ptr<client_graphics::WebGLContext> gl_context;
      if (webgl_bindings::WebGLRenderingContext::IsInstanceOf(isolate, args[1]))
      {
        auto webgl1 = webgl_bindings::WebGLRenderingContext::Unwrap(
          isolate, args[1]->ToObject(context).ToLocalChecked());
        assert(webgl1 != nullptr && "WebGLRenderingContext handle is null");
        gl_context = webgl1->handle();
      }
      else if (webgl_bindings::WebGL2RenderingContext::IsInstanceOf(isolate, args[1]))
      {
        auto webgl2 = webgl_bindings::WebGL2RenderingContext::Unwrap(
          isolate, args[1]->ToObject(context).ToLocalChecked());
        assert(webgl2 != nullptr && "WebGL2RenderingContext handle is null");
        gl_context = webgl2->handle();
      }

      // Create the native `client_xr::XRWebGLLayer` object and associate it with this wrapper
      setData(make_shared<client_xr::XRWebGLLayer>(session->handle(), gl_context));
    }

    // static
    void XRWebGLLayer::GetNativeFramebufferScaleFactor(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // TODO(yorkie): Get actual native framebuffer scale factor
      info.GetReturnValue().Set(Number::New(isolate, 1.0));
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

      // TODO(yorkie): make `framebuffer` in `handle()` return a valid WebGLFramebuffer
      auto framebufferValue = webgl_bindings::WebGLFramebuffer::NewInstance(isolate,
                                                                            handle()->framebuffer);
      info.GetReturnValue().Set(framebufferValue);
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

      info.GetReturnValue().Set(Number::New(isolate, handle()->framebufferScaleFactor));
    }

    void XRWebGLLayer::FixedFoveationSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (!value->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "fixedFoveation", "Value must be a number")));
        return;
      }

      double foveation = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(0);
      if (foveation < 0.0 || foveation > 1.0)
      {
        isolate->ThrowException(Exception::RangeError(
          MakeMethodError(isolate, "fixedFoveation", "Value must be between 0.0 and 1.0")));
        return;
      }
      handle()->framebufferScaleFactor = foveation;
    }

    // Methods

    void XRWebGLLayer::GetViewport(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getViewport", "getViewport requires 1 argument")));
        info.GetReturnValue().SetUndefined();
        return;
      }
      if (!info[0]->IsObject())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getViewport", "First argument must be an XRView object")));
        info.GetReturnValue().SetUndefined();
        return;
      }

      XRView *current_view = XRView::Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
      if (current_view == nullptr || current_view->handle() == nullptr)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getViewport", "Invalid XRView object")));
        info.GetReturnValue().SetUndefined();
        return;
      }

      auto viewport = handle()->getViewport(current_view->handle());
      info.GetReturnValue().Set(XRViewport::NewInstance(isolate, viewport));
    }
  }
}
