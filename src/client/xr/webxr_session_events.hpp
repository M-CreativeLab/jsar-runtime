#pragma once

#include <common/events_v2/event.hpp>
#include <client/dom/dom_event_target.hpp>
#include "./common.hpp"

namespace client_xr
{
  enum class XRSessionEventType
  {
    kEnd,
    kVisibilityChange,
  };

  inline dom::DOMEventType GetDOMEventType(XRSessionEventType type)
  {
    if (type == XRSessionEventType::kEnd)
      return dom::DOMEventType::XRSessionEnd;
    else
      return dom::DOMEventType::XRSessionVisibilityChange;
  }

  class XRSessionEvent : public dom::Event
  {
  public:
    XRSessionEvent(std::shared_ptr<XRSession> session, XRSessionEventType type)
        : dom::Event(dom::DOMEventConstructorType::kXRSessionEvent, GetDOMEventType(type)),
          session_(session)
    {
    }

  public:
    std::shared_ptr<XRSession> session() const { return session_; }

  private:
    std::shared_ptr<XRSession> session_;
  };

  enum class XRInputSourceEventType
  {
    kSelectStart,
    kSelectEnd,
    kSelect,
    kSqueezeStart,
    kSqueezeEnd,
    kSqueeze,
  };

  inline dom::DOMEventType GetDOMEventType(XRInputSourceEventType type)
  {
    if (type == XRInputSourceEventType::kSelectStart)
      return dom::DOMEventType::XRSessionSelectStart;
    else if (type == XRInputSourceEventType::kSelectEnd)
      return dom::DOMEventType::XRSessionSelectEnd;
    else if (type == XRInputSourceEventType::kSelect)
      return dom::DOMEventType::XRSessionSelect;
    else if (type == XRInputSourceEventType::kSqueezeStart)
      return dom::DOMEventType::XRSessionSqueezeStart;
    else if (type == XRInputSourceEventType::kSqueezeEnd)
      return dom::DOMEventType::XRSessionSqueezeEnd;
    else
      return dom::DOMEventType::XRSessionSqueeze;
  }

  class XRInputSourceEvent : public dom::Event
  {
  public:
    XRInputSourceEvent(XRInputSourceEventType type,
                       std::shared_ptr<XRFrame> frame,
                       std::shared_ptr<XRInputSource> inputSource)
        : dom::Event(dom::DOMEventConstructorType::kXRInputSourceEvent, GetDOMEventType(type)),
          frame_(frame),
          inputSource_(inputSource)
    {
    }

  public:
    std::shared_ptr<XRFrame> frame() { return frame_; }
    std::shared_ptr<XRInputSource> inputSource() { return inputSource_; }

  private:
    std::shared_ptr<XRFrame> frame_;
    std::shared_ptr<XRInputSource> inputSource_;
  };

  enum class XRInputSourcesChangeEventType
  {
    kChange,
  };

  inline dom::DOMEventType GetDOMEventType(XRInputSourcesChangeEventType type_)
  {
    return dom::DOMEventType::XRInputSourcesChange;
  }

  class XRInputSourcesChangeEvent : public dom::Event
  {
  public:
    XRInputSourcesChangeEvent(std::shared_ptr<XRSession> session,
                              std::vector<std::shared_ptr<XRInputSource>> added,
                              std::vector<std::shared_ptr<XRInputSource>> removed)
        : dom::Event(dom::DOMEventConstructorType::kXRInputSourcesChangeEvent,
                     GetDOMEventType(XRInputSourcesChangeEventType::kChange)),
          session_(session),
          added_(added),
          removed_(removed)
    {
    }
    XRInputSourcesChangeEvent(const XRInputSourcesChangeEvent &) = delete;

  public:
    /**
     * @returns The session that the input sources belong to.
     */
    std::shared_ptr<XRSession> session() { return session_; }
    /**
     * @returns The input sources that have been added.
     */
    std::vector<std::shared_ptr<XRInputSource>> &added() { return added_; }
    /**
     * @returns The input sources that have been removed.
     */
    std::vector<std::shared_ptr<XRInputSource>> &removed() { return removed_; }

  private:
    std::shared_ptr<XRSession> session_;
    std::vector<std::shared_ptr<XRInputSource>> added_;
    std::vector<std::shared_ptr<XRInputSource>> removed_;
  };
}
