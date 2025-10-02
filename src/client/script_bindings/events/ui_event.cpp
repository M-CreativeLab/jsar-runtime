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
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add property accessors
    prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "detail").ToLocalChecked(),
                                   DetailGetter,
                                   nullptr,
                                   Local<Value>(),
                                   AccessControl::DEFAULT,
                                   PropertyAttribute::ReadOnly);
  }

  v8::Local<v8::Object> UIEvent::NewInstance(v8::Isolate *isolate,
                                             std::shared_ptr<::dom::events::UIEvent> nativeEvent)
  {
    EscapableHandleScope scope(isolate);
    return nativeEvent == nullptr
             ? scope.Escape(Local<Object>())
             : scope.Escape(UIEventBase::NewInstance(isolate, nativeEvent).As<Object>());
  }

  void UIEvent::DetailGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    UIEvent *event = Unwrap(isolate, info.This());
    if (event == nullptr || event->handle() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    // long detail = event->inner()->detail();
    // info.GetReturnValue().Set(Number::New(isolate, detail));
  }
}
