#include "./all_events.hpp"

namespace script_bindings::events
{
  using namespace std;
  using namespace v8;

  void Initialize(Isolate *isolate)
  {
    Event::Initialize(isolate);
    UIEvent::Initialize(isolate);
    MouseEvent::Initialize(isolate);
    PointerEvent::Initialize(isolate);
  }

  Local<Object> MakeEvent(Isolate *isolate, dom::Event *nativeEvent)
  {
    assert(nativeEvent != nullptr && "nativeEvent must not be null");

    if (nativeEvent->isPointerEvent())
    {
      auto pTypedEvent = static_cast<dom::events::PointerEvent *>(nativeEvent);
      return PointerEvent::NewInstance(isolate, make_shared<dom::events::PointerEvent>(*pTypedEvent));
    }
    else if (nativeEvent->isMouseEvent())
    {
      auto mTypedEvent = static_cast<dom::events::MouseEvent *>(nativeEvent);
      return MouseEvent::NewInstance(isolate, make_shared<dom::events::MouseEvent>(*mTypedEvent));
    }
    else if (nativeEvent->isUIEvent())
    {
      auto uiTypedEvent = static_cast<dom::events::UIEvent *>(nativeEvent);
      return UIEvent::NewInstance(isolate, make_shared<dom::events::UIEvent>(*uiTypedEvent));
    }
    else
    {
      return Event::NewInstance(isolate, make_shared<dom::Event>(*nativeEvent));
    }
  }
}
