#include <client/xr/webxr_session_events.hpp>
#include "./all_events.hpp"

namespace endor
{
  namespace script_bindings::event_bindings
  {
    using namespace std;
    using namespace v8;

#define DOM_EVENT_CLASSES_MAP(XX) \
  XX(ErrorEvent)                  \
  XX(MessageEvent)                \
  XX(UIEvent)                     \
  XX(MouseEvent)                  \
  XX(PointerEvent)                \
  XX(WheelEvent)

#define XR_EVENT_CLASSES_MAP(XX) \
  XX(XRSessionEvent)             \
  XX(XRInputSourceEvent)         \
  XX(XRInputSourcesChangeEvent)

#define ALL_EVENT_CLASSES_MAP(XX) \
  DOM_EVENT_CLASSES_MAP(XX)       \
  XR_EVENT_CLASSES_MAP(XX)

    void Initialize(Isolate *isolate)
    {
#define XX(T) T::Initialize(isolate);
      ALL_EVENT_CLASSES_MAP(XX);
#undef XX
    }

    Local<Object> MakeEvent(Isolate *isolate, dom::Event *nativeEvent)
    {
      assert(nativeEvent != nullptr && "nativeEvent must not be null");

#define XX(T)                                                                  \
  if (nativeEvent->is##T())                                                    \
  {                                                                            \
    auto typed_event = static_cast<dom::events::T *>(nativeEvent);             \
    return T::NewInstance(isolate, make_shared<dom::events::T>(*typed_event)); \
  }

      DOM_EVENT_CLASSES_MAP(XX)
#undef XX

      // Fallback to generic Event
      return Event::NewInstance(isolate, make_shared<dom::Event>(*nativeEvent));
    }
  }
} // namespace endor
