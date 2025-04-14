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

#define WEBXR_EVENT_TYPES_MAP(XX)                             \
  XX(XRDeviceChange, "devicechange")                          \
  XX(XRSessionEnd, "XRSession.end")                           \
  XX(XRSessionSelect, "XRSession.select")                     \
  XX(XRSessionSelectEnd, "XRSession.selectend")               \
  XX(XRSessionSelectStart, "XRSession.selectstart")           \
  XX(XRSessionSqueeze, "XRSession.squeeze")                   \
  XX(XRSessionSqueezeEnd, "XRSession.squeezeend")             \
  XX(XRSessionSqueezeStart, "XRSession.squeezestart")         \
  XX(XRSessionVisibilityChange, "XRSession.visibilitychange") \
  XX(XRInputSourcesChange, "XRSession.inputsourceschange")

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
   * @param jsConstructorName The JavaScript constructor name of the event target object to compose the complete event type.
   * @returns The `DOMEventType` enum.
   */
  inline DOMEventType StringToEventType(std::string typeStr, std::optional<std::string> jsConstructorName)
  {
    std::string eventFullName = ToLowerCase(typeStr);
    if (jsConstructorName.has_value() && jsConstructorName != "")
      eventFullName = jsConstructorName.value() + "." + eventFullName;

#define XX(eventType, eventName)  \
  if (eventFullName == eventName) \
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

  enum class DOMEventConstructorType
  {
    kEvent = 0x0,
    kMouseEvent,
    kPointerEvent,
    kXRSessionEvent,
    kXRInputSourceEvent,
    kXRInputSourcesChangeEvent,
  };

  class DOMEventInit
  {
  public:
    static DOMEventInit Default()
    {
      return DOMEventInit{
          .bubbles = true,
          .cancelable = true,
          .composed = false,
      };
    }

  public:
    /**
     * A boolean value indicating whether the event bubbles. The default is `false`.
     */
    bool bubbles = true;
    /**
     * A boolean value indicating whether the event can be cancelled. The default is `false`.
     */
    bool cancelable = true;
    /**
     * A boolean value indicating whether the event will trigger listeners outside of a shadow root. The default is `false`.
     */
    bool composed = false;
  };

  class Event : public events_comm::TrEvent<DOMEventType>
  {
  public:
    using events_comm::TrEvent<DOMEventType>::TrEvent;

  public:
    /**
     * It creates a new `Event`.
     *
     * @param constructor The constructor type of the event.
     * @param type The event type.
     * @param init The event initialization options.
     */
    Event(DOMEventConstructorType constructor, DOMEventType type, DOMEventInit init = DOMEventInit::Default())
        : events_comm::TrEvent<DOMEventType>(type),
          constructor_(constructor),
          bubbles_(init.bubbles),
          cancelable_(init.cancelable),
          composed_(init.composed)
    {
    }
    Event(Event &that)
        : events_comm::TrEvent<DOMEventType>(that),
          constructor_(that.constructor_),
          bubbles_(that.bubbles_),
          cancelable_(that.cancelable_),
          composed_(that.composed_)
    {
    }

  public:
    /**
     * The constructor type of the event.
     */
    DOMEventConstructorType constructor() const { return constructor_; }
    /**
     * The bubbles read-only property of the `Event` interface indicates whether the event bubbles up through the DOM tree or not.
     */
    inline bool bubbles() { return bubbles_; }
    /**
     * The cancelable read-only property of the Event interface indicates whether the event can be canceled, and therefore prevented as if the event never happened.
     */
    inline bool cancelable() { return cancelable_; }
    /**
     * The read-only composed property of the Event interface returns a boolean value which indicates whether or not the event will propagate across the shadow DOM boundary
     * into the standard DOM.
     */
    inline bool composed() { return composed_; }
    /**
     * The type read-only property of the Event interface returns a string representing the type of the event.
     *
     * @returns The event type string, such as "click", "keydown", etc.
     */
    inline std::string typeStr() { return EventTypeToString(type); }

  public:
    void preventDefault()
    {
      if (cancelable_)
        default_prevented_ = true;
    }
    void stopPropagation()
    {
      if (bubbles_)
        bubbles_ = false;
    }

  private:
    DOMEventConstructorType constructor_ = DOMEventConstructorType::kEvent;
    bool bubbles_;
    bool cancelable_;
    bool composed_;
    bool default_prevented_ = false;
  };

  class EventListener : public events_comm::TrEventListener<DOMEventType, Event>
  {
  public:
    using events_comm::TrEventListener<DOMEventType, Event>::TrEventListener;
  };

  class DOMEventTarget : public events_comm::TrEventTarget<DOMEventType, Event>
  {
  public:
    DOMEventTarget() = default;
  };
}
