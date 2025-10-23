#include "./ui_event.hpp"

namespace script_bindings::event_bindings
{
  using namespace std;
  using namespace v8;

  // static
  void UIEvent::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);

    // Set up the instance template
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    // Add property accessors
    InstanceReadonlyAccessor(isolate, prototype, "detail", &UIEvent::DetailGetter);
  }

  v8::Local<v8::Object> UIEvent::NewInstance(v8::Isolate *isolate,
                                             std::shared_ptr<::dom::events::UIEvent> nativeEvent)
  {
    EscapableHandleScope scope(isolate);
    return nativeEvent == nullptr
             ? scope.Escape(Local<Object>())
             : scope.Escape(UIEventBase::NewInstance(isolate, nativeEvent).As<Object>());
  }

  void UIEvent::DetailGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().SetUndefined();
  }
}
