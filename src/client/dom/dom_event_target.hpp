#pragma once

#include <string>
#include <memory>
#include "common/utility.hpp"
#include "common/events_v2/event_target.hpp"

namespace dom
{
#define NODE_EVENT_TYPES_MAP(XX) \
  XX(SelectStart, "selectstart")

#define ELEMENT_EVENT_TYPES_MAP(XX)            \
  XX(AnimationCancel, "animationcancel")       \
  XX(AnimationEnd, "animationend")             \
  XX(AnimationIteration, "animationiteration") \
  XX(AnimationStart, "animationstart")         \
  XX(AuxClick, "auxclick")                     \
  XX(BeforeInput, "beforeinput")               \
  XX(Blur, "blur")                             \
  XX(Click, "click")                           \
  XX(CompositionEnd, "compositionend")         \
  XX(CompositionStart, "compositionstart")     \
  XX(CompositionUpdate, "compositionupdate")   \
  XX(ContextMenu, "contextmenu")               \
  XX(Copy, "copy")                             \
  XX(Cut, "cut")                               \
  XX(DoubleClick, "dbclick")                   \
  XX(Input, "input")                           \
  XX(KeyDown, "keydown")                       \
  XX(KeyUp, "keyup")                           \
  XX(MouseDown, "mousedown")                   \
  XX(MouseEnter, "mouseenter")                 \
  XX(MouseLeave, "mouseleave")                 \
  XX(MouseMove, "mousemove")                   \
  XX(MouseOut, "mouseout")                     \
  XX(MouseOver, "mouseover")                   \
  XX(MouseUp, "mouseup")                       \
  XX(Paste, "paste")                           \
  XX(PointerCancel, "pointercancel")           \
  XX(PointerDown, "pointerdown")               \
  XX(PointerEnter, "pointerenter")             \
  XX(PointerLeave, "pointerleave")             \
  XX(PointerMove, "pointermove")               \
  XX(PointerOut, "pointerout")                 \
  XX(PointerOver, "pointerover")               \
  XX(PointerUp, "pointerup")                   \
  XX(Scroll, "scroll")                         \
  XX(ScrollEnd, "scrollend")                   \
  XX(SecurityPolicyViolation, "securitypolicyviolation")

#define HTMLELEMENT_EVENT_TYPES_MAP(XX) \
  XX(BeforeToggle, "beforetoggle")      \
  XX(Cancel, "cancel")                  \
  XX(Change, "change")                  \
  XX(Drag, "drag")                      \
  XX(DragEnd, "dragend")                \
  XX(DragEnter, "dragenter")            \
  XX(DragLeave, "dragleave")            \
  XX(DragOver, "dragover")              \
  XX(DragStart, "dragstart")            \
  XX(Drop, "drop")                      \
  XX(Error, "error")                    \
  XX(Load, "load")                      \
  XX(Toggle, "toggle")

#define HTMLMEDIAELEMENT_EVENT_TYPES_MAP(XX) \
  XX(Abort, "abort")                         \
  XX(CanPlay, "canplay")                     \
  XX(CanPlayThrough, "canplaythrough")       \
  XX(DurationChange, "durationchange")       \
  XX(Emptied, "emptied")                     \
  XX(Ended, "ended")                         \
  XX(LoadedData, "loadeddata")               \
  XX(LoadedMetadata, "loadedmetadata")       \
  XX(LoadStart, "loadstart")                 \
  XX(Pause, "pause")                         \
  XX(Play, "play")                           \
  XX(Playing, "playing")                     \
  XX(Progress, "progress")                   \
  XX(RateChange, "ratechange")               \
  XX(Seeked, "seeked")                       \
  XX(Seeking, "seeking")                     \
  XX(Stalled, "stalled")                     \
  XX(Suspend, "suspend")                     \
  XX(TimeUpdate, "timeupdate")               \
  XX(VolumeChange, "volumechange")           \
  XX(Waiting, "waiting")

#define DOCUMENT_EVENT_TYPES_MAP(XX)       \
  XX(DOMContentLoaded, "domcontentloaded") \
  XX(FullscreenChange, "fullscreenchange") \
  XX(FullscreenError, "fullscreenerror")   \
  XX(SelectionChange, "selectionchange")   \
  XX(VisibilityChange, "visibilitychange")

#define CANVAS_EVENT_TYPES_MAP(XX)                           \
  XX(ContextLost, "contextlost")                             \
  XX(ContextRestored, "contextrestored")                     \
  XX(WebGLContextCreationError, "webglcontextcreationerror") \
  XX(WebGLContextLost, "webglcontextlost")                   \
  XX(WebGLContextRestored, "webglcontextrestored")

#define WEBXR_EVENT_TYPES_MAP(XX)                   \
  XX(XRDeviceChange, "devicechange")                \
  XX(XRSessionEnd, "end")                           \
  XX(XRSessionSelect, "select")                     \
  XX(XRSessionSelectEnd, "selectend")               \
  XX(XRSessionSelectStart, "selectstart")           \
  XX(XRSessionSqueeze, "squeeze")                   \
  XX(XRSessionSqueezeEnd, "squeezeend")             \
  XX(XRSessionSqueezeStart, "squeezestart")         \
  XX(XRSessionVisibilityChange, "visibilitychange") \
  XX(XRInputSourcesChange, "inputsourceschange")

#define DOM_EVENT_TYPES_MAP(XX)        \
  NODE_EVENT_TYPES_MAP(XX)             \
  ELEMENT_EVENT_TYPES_MAP(XX)          \
  HTMLELEMENT_EVENT_TYPES_MAP(XX)      \
  HTMLMEDIAELEMENT_EVENT_TYPES_MAP(XX) \
  DOCUMENT_EVENT_TYPES_MAP(XX)         \
  CANVAS_EVENT_TYPES_MAP(XX)           \
  WEBXR_EVENT_TYPES_MAP(XX)

  enum class DOMEventType
  {
#define XX(eventType, _) \
  eventType,
    DOM_EVENT_TYPES_MAP(XX)
#undef XX
  };

  /**
   * Convert the event type string such as "click" to the `DOMEventType` enum.
   *
   * @param typeStr The event type string.
   * @returns The `DOMEventType` enum.
   */
  inline DOMEventType StringToEventType(std::string typeStr)
  {
#define XX(eventType, eventName)         \
  if (ToLowerCase(typeStr) == eventName) \
    return DOMEventType::eventType;

    DOM_EVENT_TYPES_MAP(XX)
#undef XX

    throw std::invalid_argument("Invalid event type string: " + typeStr);
  }

  /**
   * Convert the `DOMEventType` enum to the event type string such as "click".
   *
   * @param eventType The `DOMEventType` enum.
   * @returns The event type string.
   */
  inline std::string EventTypeToString(DOMEventType eventType)
  {
    switch (eventType)
    {
#define XX(eventType, eventName) \
  case DOMEventType::eventType:  \
    return eventName;

      DOM_EVENT_TYPES_MAP(XX)
#undef XX

    default:
      throw std::invalid_argument("Invalid event type: " + std::to_string(static_cast<int>(eventType)));
    }
  }

  using Event = events_comm::TrEvent<DOMEventType>;
  using EventListener = events_comm::TrEventListener<DOMEventType, Event>;

  class DOMEventTarget : public events_comm::TrEventTarget<DOMEventType>
  {
  public:
    DOMEventTarget() = default;
  };
}
