#pragma once

#include <string>
#include <memory>
#include <optional>
#include <chrono>

#include <common/utility.hpp>
#include <common/events_v2/event_target.hpp>
#include <client/scripting_base/v8_object_holder.hpp>

namespace dom
{
  // Forward declarations
  class DOMEventTarget;

  /// Event Constructors

#define DOM_EVENT_CONSTRUCTOR_TYPE_MAP(XX) \
  XX(Event)                                \
  XX(MouseEvent)                           \
  XX(PointerEvent)                         \
  XX(XRSessionEvent)                       \
  XX(XRInputSourceEvent)                   \
  XX(XRInputSourcesChangeEvent)

  enum class DOMEventConstructorType
  {
#define XX(CONSTRUCTOR_TYPE) k##CONSTRUCTOR_TYPE,
    DOM_EVENT_CONSTRUCTOR_TYPE_MAP(XX)
#undef XX
  };

  // Convert constructor type to the string such as "MouseEvent".
  inline std::string to_string(const DOMEventConstructorType constructorType)
  {
    switch (constructorType)
    {
#define XX(CONSTRUCTOR_TYPE)                         \
  case DOMEventConstructorType::k##CONSTRUCTOR_TYPE: \
    return #CONSTRUCTOR_TYPE;

      DOM_EVENT_CONSTRUCTOR_TYPE_MAP(XX)
#undef XX

    default:
      throw std::invalid_argument("Invalid event type: " + std::to_string(static_cast<int>(constructorType)));
    }
  }

  // Parse a string to the DOMEventConstructorType.
  inline DOMEventConstructorType from(const std::string &str)
  {
#define XX(CONSTRUCTOR_TYPE)    \
  if (str == #CONSTRUCTOR_TYPE) \
    return DOMEventConstructorType::k##CONSTRUCTOR_TYPE;

    DOM_EVENT_CONSTRUCTOR_TYPE_MAP(XX)
#undef XX
    throw std::invalid_argument("Invalid event type: " + str);
  }

#undef DOM_EVENT_CONSTRUCTOR_TYPE_MAP

  /// DOM `EventTarget` objects
  /// used to compose the complete event name.

#define DOM_EVENT_TARGET_TYPE_MAP(XX) \
  XX(XRSession)

  enum class DOMEventTargetType
  {
    kEventTarget = 0x0,
#define XX(DOMEventTargetType) k##DOMEventTargetType,
    DOM_EVENT_TARGET_TYPE_MAP(XX)
#undef XX
  };

  /**
   * Convert event target type to the string such as "XRSession".
   */
  inline std::string to_string(const DOMEventTargetType type)
  {
    switch (type)
    {
#define XX(EVENT_TARGET_TYPE)                    \
  case DOMEventTargetType::k##EVENT_TARGET_TYPE: \
    return #EVENT_TARGET_TYPE;

      DOM_EVENT_TARGET_TYPE_MAP(XX)
#undef XX

    default:
      throw std::invalid_argument("Invalid event type: " + std::to_string(static_cast<int>(type)));
    }
  }

#undef DOM_EVENT_TARGET_TYPE_MAP

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
   * @param fromEventTarget The event target type that the event is from.
   * @returns The `DOMEventType` enum.
   */
  inline std::optional<DOMEventType> StringToEventType(std::string typeStr, DOMEventTargetType fromEventTarget)
  {
    std::string eventFullName = ToLowerCase(typeStr);
    if (fromEventTarget != DOMEventTargetType::kEventTarget)
      eventFullName = to_string(fromEventTarget) + "." + eventFullName;

#define XX(eventType, eventName)  \
  if (eventFullName == eventName) \
    return DOMEventType::eventType;

    DOM_EVENT_TYPES_MAP(XX)
#undef XX

    return std::nullopt;
  }

  /**
   * Convert the `DOMEventType` enum to the event type string such as "click".
   *
   * @param eventType The `DOMEventType` enum.
   * @param removePrefix Should remove the event prefix.
   * @returns The event type string.
   */
  inline std::string EventTypeToString(DOMEventType eventType, bool removePrefix = false)
  {
    std::string eventName;
    switch (eventType)
    {
#define XX(TYPE, NAME)     \
  case DOMEventType::TYPE: \
    eventName = NAME;      \
    break;

      DOM_EVENT_TYPES_MAP(XX)
#undef XX

    default:
      throw std::invalid_argument("Invalid event type: " + std::to_string(static_cast<int>(eventType)));
    }

    if (removePrefix)
    {
      size_t pos = eventName.find('.');
      if (pos != std::string::npos)
        eventName.erase(0, pos + 1);
    }
    return eventName;
  }

  /**
   * Event phase constants according to DOM specification.
   */
  enum class EventPhase : unsigned short
  {
    NONE = 0,
    CAPTURING_PHASE = 1,
    AT_TARGET = 2,
    BUBBLING_PHASE = 3
  };

  /**
   * DOMHighResTimeStamp - timestamp in milliseconds since time origin.
   */
  using DOMHighResTimeStamp = double;

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

  /**
   * The Event interface represents an event which takes place in the DOM.
   * This implementation follows the DOM specification.
   * 
   * @see https://dom.spec.whatwg.org/#event
   * @see https://developer.mozilla.org/en-US/docs/Web/API/Event
   */
  class Event : public events_comm::TrEvent<DOMEventType>,
                public scripting_base::JSObjectHolder
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
        : events_comm::TrEvent<DOMEventType>(type)
        , scripting_base::JSObjectHolder()
        , constructor_(constructor)
        , bubbles_(init.bubbles)
        , cancelable_(init.cancelable)
        , composed_(init.composed)
        , event_phase_(EventPhase::NONE)
        , time_stamp_(getCurrentTimeStamp())
        , default_prevented_(false)
        , stop_propagation_flag_(false)
        , stop_immediate_propagation_flag_(false)
    {
    }

    Event(Event &that)
        : events_comm::TrEvent<DOMEventType>(that)
        , scripting_base::JSObjectHolder(that)
        , constructor_(that.constructor_)
        , bubbles_(that.bubbles_)
        , cancelable_(that.cancelable_)
        , composed_(that.composed_)
        , event_phase_(that.event_phase_)
        , time_stamp_(that.time_stamp_)
        , default_prevented_(that.default_prevented_)
        , stop_propagation_flag_(that.stop_propagation_flag_)
        , stop_immediate_propagation_flag_(that.stop_immediate_propagation_flag_)
        , target_(that.target_)
        , current_target_(that.current_target_)
    {
    }

  public:
    virtual bool isUIEvent() const
    {
      return false;
    }
    virtual bool isMouseEvent() const
    {
      return false;
    }
    virtual bool isInputEvent() const
    {
      return false;
    }
    virtual bool isKeyboardEvent() const
    {
      return false;
    }
    virtual bool isPointerEvent() const
    {
      return false;
    }
    virtual bool isXRSessionEvent() const
    {
      return false;
    }
    virtual bool isXRInputSourceEvent() const
    {
      return false;
    }
    virtual bool isXRInputSourcesChangeEvent() const
    {
      return false;
    }

  public:
    /**
     * The constructor type of the event.
     */
    DOMEventConstructorType constructor() const
    {
      return constructor_;
    }

    /**
     * The bubbles read-only property of the `Event` interface indicates whether the event bubbles up through the DOM tree or not.
     */
    inline bool bubbles() const
    {
      return bubbles_;
    }

    /**
     * The cancelable read-only property of the Event interface indicates whether the event can be canceled, and therefore prevented as if the event never happened.
     */
    inline bool cancelable() const
    {
      return cancelable_;
    }

    /**
     * The read-only composed property of the Event interface returns a boolean value which indicates whether or not the event will propagate across the shadow DOM boundary
     * into the standard DOM.
     */
    inline bool composed() const
    {
      return composed_;
    }

    /**
     * The type read-only property of the Event interface returns a string representing the type of the event.
     *
     * @returns The event type string, such as "click", "keydown", etc.
     */
    inline std::string typeStr() const
    {
      return EventTypeToString(type, true);
    }

    /**
     * The target read-only property of the Event interface is a reference to the object onto which the event was dispatched.
     * @returns A reference to the EventTarget to which the event was originally dispatched.
     */
    std::shared_ptr<DOMEventTarget> target() const
    {
      return target_;
    }

    /**
     * The currentTarget read-only property of the Event interface identifies the current target for the event,
     * as the event traverses the DOM. It always refers to the element the event handler has been attached to,
     * as opposed to Event.target, which identifies the element on which the event occurred and which may be its descendant.
     * @returns A reference to the EventTarget that is currently processing the event.
     */
    std::shared_ptr<DOMEventTarget> currentTarget() const
    {
      return current_target_;
    }

    /**
     * The eventPhase read-only property of the Event interface indicates which phase of the event flow is currently being evaluated.
     * @returns The phase of the event flow that is currently being processed.
     */
    EventPhase eventPhase() const
    {
      return event_phase_;
    }

    /**
     * The timeStamp read-only property of the Event interface returns the time (in milliseconds) at which the event was created.
     * @returns The time in milliseconds at which the event was created.
     */
    DOMHighResTimeStamp timeStamp() const
    {
      return time_stamp_;
    }

    /**
     * The defaultPrevented read-only property of the Event interface returns a boolean value indicating whether or not the call to Event.preventDefault() canceled the event.
     * @returns true if Event.preventDefault() was called for this event, false otherwise.
     */
    bool defaultPrevented() const
    {
      return default_prevented_;
    }

  public:
    /**
     * The preventDefault() method of the Event interface tells the user agent that if the event does not get explicitly handled,
     * its default action should not be taken as it normally would be.
     */
    void preventDefault()
    {
      if (cancelable_)
        default_prevented_ = true;
    }

    /**
     * The stopPropagation() method of the Event interface prevents further propagation of the current event
     * in the capturing and bubbling phases.
     */
    void stopPropagation()
    {
      stop_propagation_flag_ = true;
    }

    /**
     * The stopImmediatePropagation() method of the Event interface prevents other listeners of the same event from being called.
     */
    void stopImmediatePropagation()
    {
      stop_immediate_propagation_flag_ = true;
      stop_propagation_flag_ = true;
    }

  public:
    /**
     * Internal methods for event dispatch - these should only be called by the event dispatcher.
     * Note: These are public for technical reasons but should be treated as internal API.
     */
    void setTarget(std::shared_ptr<DOMEventTarget> target)
    {
      target_ = target;
    }

    void setCurrentTarget(std::shared_ptr<DOMEventTarget> currentTarget)
    {
      current_target_ = currentTarget;
    }

    void setEventPhase(EventPhase phase)
    {
      event_phase_ = phase;
    }

    bool stopPropagationFlag() const
    {
      return stop_propagation_flag_;
    }

    bool stopImmediatePropagationFlag() const
    {
      return stop_immediate_propagation_flag_;
    }

  private:
    /**
     * Get current timestamp in milliseconds since an arbitrary time origin.
     * This implementation uses steady_clock to provide monotonic time.
     */
    static DOMHighResTimeStamp getCurrentTimeStamp()
    {
      static auto time_origin = std::chrono::steady_clock::now();
      auto now = std::chrono::steady_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - time_origin);
      return static_cast<double>(duration.count()) / 1e6; // Convert to milliseconds
    }

  private:
    DOMEventConstructorType constructor_ = DOMEventConstructorType::kEvent;
    bool bubbles_;
    bool cancelable_;
    bool composed_;
    EventPhase event_phase_;
    DOMHighResTimeStamp time_stamp_;
    bool default_prevented_;
    bool stop_propagation_flag_;
    bool stop_immediate_propagation_flag_;

    // Event targets
    std::shared_ptr<DOMEventTarget> target_;
    std::shared_ptr<DOMEventTarget> current_target_;
  };
}
