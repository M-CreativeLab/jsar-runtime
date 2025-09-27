#include "./mouse_event.hpp"

namespace script_bindings::event_bindings
{
  using namespace std;
  using namespace v8;

  // static
  void MouseEvent::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);

    // Set up the instance template
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add property accessors
    // (No additional properties for MouseEvent beyond UIEvent for now)
  }

  v8::Local<v8::Object> MouseEvent::NewInstance(v8::Isolate *isolate,
                                                std::shared_ptr<::dom::events::MouseEvent> nativeEvent)
  {
    EscapableHandleScope scope(isolate);
    return nativeEvent == nullptr
             ? scope.Escape(Local<Object>())
             : scope.Escape(MouseEventBase::NewInstance(isolate, nativeEvent).As<Object>());
  }
}
