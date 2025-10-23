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
    assert(nativeEvent != nullptr && "nativeEvent must not be null");
    return scope.Escape(MouseEventBase::NewInstance(isolate, nativeEvent).As<Object>());
  }

  MouseEvent::MouseEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
      : MouseEventBase(isolate, args)
  {
    HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeConstructorError(isolate, "1 argument required, but only 0 present.")));
      return;
    }

    // TODO(yorkie): implement this.
    // setInner(make_shared<::dom::events::MouseEvent>());
  }
}
