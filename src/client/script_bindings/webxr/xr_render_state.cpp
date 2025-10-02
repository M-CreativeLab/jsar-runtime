#include <iostream>
#include "./xr_render_state.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    void XRRenderState::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "baseLayer", &XRRenderState::BaseLayerGetter);
      InstanceReadonlyAccessor(isolate, instance, "depthFar", &XRRenderState::DepthFarGetter);
      InstanceReadonlyAccessor(isolate, instance, "depthNear", &XRRenderState::DepthNearGetter);
      InstanceReadonlyAccessor(isolate,
                               instance,
                               "inlineVerticalFieldOfView",
                               &XRRenderState::InlineVerticalFieldOfViewGetter);
    }

    XRRenderState::XRRenderState(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRRenderStateBase(isolate, args, true)
    {
    }

    // Property getters

    void XRRenderState::BaseLayerGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRRenderState *renderState = Unwrap(isolate, info.This());
      if (renderState == nullptr || renderState->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual base layer from native render state
      cout << "renderState.baseLayer getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRRenderState::DepthFarGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto depthFar = handle()->depthFar;
      info.GetReturnValue().Set(Number::New(isolate, depthFar));
    }

    void XRRenderState::DepthNearGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto depthNear = handle()->depthNear;
      info.GetReturnValue().Set(Number::New(isolate, depthNear));
    }

    void XRRenderState::InlineVerticalFieldOfViewGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto fov = handle()->inlineVerticalFieldOfView;
      info.GetReturnValue().Set(Number::New(isolate, fov));
    }
  }
}
