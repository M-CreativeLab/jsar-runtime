#include "./all_events.hpp"
#include <client/xr/webxr_session_events.hpp>

namespace script_bindings::event_bindings
{
  using namespace std;
  using namespace v8;

  void Initialize(Isolate *isolate)
  {
    // Initialize all event class constructors
    UIEvent::Initialize(isolate);
    MouseEvent::Initialize(isolate);
    PointerEvent::Initialize(isolate);
    XRSessionEvent::Initialize(isolate);
    XRInputSourceEvent::Initialize(isolate);
    XRInputSourcesChangeEvent::Initialize(isolate);
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

  Local<Object> MakeXREvent(Isolate *isolate, void *nativeEvent, const string &eventType)
  {
    if (!nativeEvent)
    {
      return Local<Object>();
    }

    if (eventType == "session")
    {
      auto sessionEvent = static_cast<client_xr::XRSessionEvent *>(nativeEvent);
      return XRSessionEvent::NewInstance(isolate, shared_ptr<client_xr::XRSessionEvent>(sessionEvent, [](client_xr::XRSessionEvent *) {}));
    }
    else if (eventType == "inputsource")
    {
      auto inputSourceEvent = static_cast<client_xr::XRInputSourceEvent *>(nativeEvent);
      return XRInputSourceEvent::NewInstance(isolate, shared_ptr<client_xr::XRInputSourceEvent>(inputSourceEvent, [](client_xr::XRInputSourceEvent *) {}));
    }
    else if (eventType == "inputsourceschange")
    {
      auto inputSourcesChangeEvent = static_cast<client_xr::XRInputSourcesChangeEvent *>(nativeEvent);
      return XRInputSourcesChangeEvent::NewInstance(isolate, shared_ptr<client_xr::XRInputSourcesChangeEvent>(inputSourcesChangeEvent, [](client_xr::XRInputSourcesChangeEvent *) {}));
    }

    return Local<Object>();
  }
}
