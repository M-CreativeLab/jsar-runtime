#include "./xr_viewport.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // static
    void XRViewport::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "x").ToLocalChecked(),
                                    XGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "y").ToLocalChecked(),
                                    YGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                                    WidthGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                                    HeightGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
    }

    // static
    Local<Object> XRViewport::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRViewport> nativeViewport)
    {
      EscapableHandleScope scope(isolate);

      if (nativeViewport == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>::NewInstance(isolate, nativeViewport).As<Object>());
    }

    // static
    Local<Function> XRViewport::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>::Initialize(isolate);
    }

    XRViewport::XRViewport(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>(isolate, args)
    {
    }

    // Property getters

    // static
    void XRViewport::XGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRViewport *viewport = scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>::Unwrap(info.This());
      if (viewport == nullptr || viewport->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
        return;
      }

      // TODO: Implement actual viewport x coordinate from native
      info.GetReturnValue().Set(Number::New(isolate, 0));
    }

    // static
    void XRViewport::YGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRViewport *viewport = scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>::Unwrap(info.This());
      if (viewport == nullptr || viewport->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
        return;
      }

      // TODO: Implement actual viewport y coordinate from native
      info.GetReturnValue().Set(Number::New(isolate, 0));
    }

    // static
    void XRViewport::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRViewport *viewport = scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>::Unwrap(info.This());
      if (viewport == nullptr || viewport->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
        return;
      }

      // TODO: Implement actual viewport width from native
      info.GetReturnValue().Set(Number::New(isolate, 1920));
    }

    // static
    void XRViewport::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRViewport *viewport = scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>::Unwrap(info.This());
      if (viewport == nullptr || viewport->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
        return;
      }

      // TODO: Implement actual viewport height from native
      info.GetReturnValue().Set(Number::New(isolate, 1080));
    }
  }
}