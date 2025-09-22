#include "./xr_render_state.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr
  {
    // static
    void XRRenderState::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "baseLayer").ToLocalChecked(),
                                    BaseLayerGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "depthFar").ToLocalChecked(),
                                    DepthFarGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "depthNear").ToLocalChecked(),
                                    DepthNearGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "inlineVerticalFieldOfView").ToLocalChecked(),
                                    InlineVerticalFieldOfViewGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
    }

    // static
    Local<Object> XRRenderState::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRRenderState> nativeRenderState)
    {
      EscapableHandleScope scope(isolate);

      if (nativeRenderState == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>::NewInstance(isolate, nativeRenderState).As<Object>());
    }

    // static
    Local<Function> XRRenderState::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>::Initialize(isolate);
    }

    XRRenderState::XRRenderState(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>(isolate, args)
    {
    }

    // Property getters

    // static
    void XRRenderState::BaseLayerGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRenderState *renderState = scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>::Unwrap(info.This());
      if (renderState == nullptr || renderState->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual base layer from native render state
      cout << "renderState.baseLayer getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRRenderState::DepthFarGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRenderState *renderState = scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>::Unwrap(info.This());
      if (renderState == nullptr || renderState->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 1000.0));
        return;
      }

      // TODO: Get actual depth far from native render state
      info.GetReturnValue().Set(Number::New(isolate, 1000.0));
    }

    // static
    void XRRenderState::DepthNearGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRenderState *renderState = scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>::Unwrap(info.This());
      if (renderState == nullptr || renderState->inner() == nullptr)
      {
        info.GetReturnValue().Set(Number::New(isolate, 0.1));
        return;
      }

      // TODO: Get actual depth near from native render state
      info.GetReturnValue().Set(Number::New(isolate, 0.1));
    }

    // static
    void XRRenderState::InlineVerticalFieldOfViewGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRenderState *renderState = scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>::Unwrap(info.This());
      if (renderState == nullptr || renderState->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Get actual inline vertical field of view from native render state
      cout << "renderState.inlineVerticalFieldOfView getter called" << endl;
      info.GetReturnValue().SetNull();
    }
  }
}