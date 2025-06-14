#pragma once

#include <memory>

namespace client_layout
{
  class LayoutObject;
  class HitTestRequest
  {
  public:
    enum RequestType
    {
      kReadOnly = 1 << 1,
      kActive = 1 << 2,
      kMove = 1 << 3,
      kRelease = 1 << 4,
      kIgnoreClipping = 1 << 5,
      kSVGClipContent = 1 << 6,
      kTouchEvent = 1 << 7,
      kAllowChildFrameContent = 1 << 8,
      kChildFrameHitTest = 1 << 9,
      kIgnorePointerEventsNone = 1 << 10,
      // Collect a list of nodes instead of just one.
      // (This is for elementsFromPoint and rect-based tests).
      kListBased = 1 << 11,
      // When using list-based testing, this flag causes us to continue hit
      // testing after a hit has been found.
      kPenetratingList = 1 << 12,
      kAvoidCache = 1 << 13,
      kIgnoreZeroOpacityObjects = 1 << 14,
      kHitTestVisualOverflow = 1 << 15,
      kHitNodeCbWithId = 1 << 16,
    };

    typedef unsigned HitTestRequestType;

  public:
    HitTestRequest(HitTestRequestType, std::shared_ptr<LayoutObject> stopNode = nullptr);

  public:
    bool readOnly() const
    {
      return request_type_ & kReadOnly;
    }
    bool active() const
    {
      return request_type_ & kActive;
    }
    bool move() const
    {
      return request_type_ & kMove;
    }
    bool release() const
    {
      return request_type_ & kRelease;
    }
    bool ignoreClipping() const
    {
      return request_type_ & kIgnoreClipping;
    }
    bool svgClipContent() const
    {
      return request_type_ & kSVGClipContent;
    }
    bool touchEvent() const
    {
      return request_type_ & kTouchEvent;
    }
    bool allowsChildFrameContent() const
    {
      return request_type_ & kAllowChildFrameContent;
    }
    bool isChildFrameHitTest() const
    {
      return request_type_ & kChildFrameHitTest;
    }
    bool isHitTestVisualOverflow() const
    {
      return request_type_ & kHitTestVisualOverflow;
    }
    bool ignorePointerEventsNone() const
    {
      return request_type_ & kIgnorePointerEventsNone;
    }
    bool listBased() const
    {
      return request_type_ & kListBased;
    }
    bool penetratingList() const
    {
      return request_type_ & kPenetratingList;
    }
    bool avoidCache() const
    {
      return request_type_ & kAvoidCache;
    }

    bool touchMove() const
    {
      return move() && touchEvent();
    }

    HitTestRequestType getType() const
    {
      return request_type_;
    }
    std::shared_ptr<LayoutObject> getStopNode() const
    {
      return stop_node_.lock();
    }

  private:
    HitTestRequestType request_type_;
    std::weak_ptr<LayoutObject> stop_node_;
  };
}
