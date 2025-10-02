#include "./xr_viewport.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    void XRViewport::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "x", &XRViewport::XGetter);
      InstanceReadonlyAccessor(isolate, instance, "y", &XRViewport::YGetter);
      InstanceReadonlyAccessor(isolate, instance, "width", &XRViewport::WidthGetter);
      InstanceReadonlyAccessor(isolate, instance, "height", &XRViewport::HeightGetter);
    }

    XRViewport::XRViewport(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRViewportBase(isolate, args)
    {
    }

    // Property getters

    void XRViewport::XGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate,
                                            handle()->x));
    }

    void XRViewport::YGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate,
                                            handle()->y));
    }

    void XRViewport::WidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate,
                                            handle()->width));
    }

    void XRViewport::HeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate,
                                            handle()->height));
    }
  }
}
