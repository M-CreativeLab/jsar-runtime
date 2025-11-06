#include "./wheel_event.hpp"

namespace endor
{
  namespace script_bindings::event_bindings
  {
    using namespace std;
    using namespace v8;

    void WheelEvent::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "deltaX", &WheelEvent::DeltaXGetter);
      InstanceReadonlyAccessor(isolate, instance, "deltaY", &WheelEvent::DeltaYGetter);
      InstanceReadonlyAccessor(isolate, instance, "deltaZ", &WheelEvent::DeltaZGetter);
      InstanceReadonlyAccessor(isolate, instance, "deltaMode", &WheelEvent::DeltaModeGetter);

      // Add delta mode constants
      tpl->Set(isolate, "DOM_DELTA_PIXEL", Number::New(isolate, dom::events::WheelEvent::DOM_DELTA_PIXEL));
      tpl->Set(isolate, "DOM_DELTA_LINE", Number::New(isolate, dom::events::WheelEvent::DOM_DELTA_LINE));
      tpl->Set(isolate, "DOM_DELTA_PAGE", Number::New(isolate, dom::events::WheelEvent::DOM_DELTA_PAGE));
    }

    v8::Local<v8::Object> WheelEvent::NewInstance(v8::Isolate *isolate,
                                                  std::shared_ptr<::endor::dom::events::WheelEvent> nativeEvent)
    {
      EscapableHandleScope scope(isolate);
      assert(nativeEvent != nullptr && "nativeEvent must not be null");
      return scope.Escape(WheelEventBase::NewInstance(isolate, nativeEvent).As<Object>());
    }

    WheelEvent::WheelEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : WheelEventBase(isolate, args)
    {
      HandleScope scope(isolate);

      if (args.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate, "1 argument required, but only 0 present.")));
        return;
      }

      // TODO: Implement constructor with WheelEventInit dictionary
    }

    void WheelEvent::DeltaXGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate, handle()->deltaX()));
    }

    void WheelEvent::DeltaYGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate, handle()->deltaY()));
    }

    void WheelEvent::DeltaZGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate, handle()->deltaZ()));
    }

    void WheelEvent::DeltaModeGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate, handle()->deltaMode()));
    }
  }
} // namespace endor
