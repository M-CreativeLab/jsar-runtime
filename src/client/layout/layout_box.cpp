#include <client/dom/node.hpp>
#include <client/dom/element.hpp>

#include "./layout_box.hpp"
#include "./layout_text.hpp"

namespace client_layout
{
  using namespace std;

  LayoutBox::LayoutBox(shared_ptr<dom::Node> node)
      : LayoutBoxModelObject(node)
  {
  }

  shared_ptr<LayoutBox> LayoutBox::firstChildBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(slowFirstChild());
  }

  shared_ptr<LayoutBox> LayoutBox::lastChildBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(slowLastChild());
  }

  shared_ptr<LayoutBox> LayoutBox::nextSiblingBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(nextSibling());
  }

  shared_ptr<LayoutBox> LayoutBox::previousSiblingBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(prevSibling());
  }

  shared_ptr<LayoutBox> LayoutBox::parentBox() const
  {
    return dynamic_pointer_cast<LayoutBox>(parent());
  }

  glm::vec3 LayoutBox::size() const
  {
    if (!hasValidCachedGeometry())
    {
      const_cast<LayoutBox *>(this)->setHasValidCachedGeometry(true);
      const_cast<LayoutBox *>(this)->frame_size_ = computeSize();
    }
    return frame_size_;
  }

  geometry::Rect<float> LayoutBox::scrollableOverflowRect() const
  {
    return scrollableOverflowIsSet()
               ? overflow_->scrollableOverflow->scrollableOverflowRect()
               : noOverflowRect();
  }

  bool LayoutBox::hasTopOverflow() const
  {
    if (isHorizontalWritingMode())
      return false;

    // TODO(yorkie): implement ComputedStyle to improve the performance.
    auto elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyle();
    string direction = elementStyle.hasProperty("direction")
                           ? elementStyle.getPropertyValue("direction")
                           : "ltr";
    string writingMode = elementStyle.hasProperty("writing-mode")
                             ? elementStyle.getPropertyValue("writing-mode")
                             : "horizontal-tb";
    if (writingMode == "horizontal-tb")
      return false;
    else if (writingMode == "sideways-lr")
      return direction == "ltr";
    else if (writingMode == "vertical-rl" ||
             writingMode == "vertical-lr" ||
             writingMode == "sideways-rl")
      return direction != "ltr";
    else
      return false;
  }

  bool LayoutBox::hasLeftOverflow() const
  {
    // TODO(yorkie): implement ComputedStyle to improve the performance.
    auto elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyle();
    string direction = elementStyle.hasProperty("direction")
                           ? elementStyle.getPropertyValue("direction")
                           : "ltr";

    if (isHorizontalWritingMode())
      return direction != "ltr";

    string writingMode = elementStyle.hasProperty("writing-mode")
                             ? elementStyle.getPropertyValue("writing-mode")
                             : "horizontal-tb";
    if (writingMode == "horizontal-tb")
      return direction != "ltr";
    else if (writingMode == "vertical-lr" || writingMode == "vertical-rl")
      return false;
    else if (writingMode == "vertical-rl" || writingMode == "sideways-rl")
      return true;
    else
      return false;
  }

  void LayoutBox::setScrollableOverflowFromLayoutResults()
  {
    if (overflow_)
      overflow_->scrollableOverflow.reset();

    if (isLayoutReplaced())
      return;

    // TODO(yorkie): implement this.
  }

  float LayoutBox::clientLeft() const
  {
    return borderLeft();
  }

  float LayoutBox::clientTop() const
  {
    return borderTop();
  }

  float LayoutBox::clientWidth() const
  {
    float width = size().x;
    return width - borderLeft() - borderRight();
  }

  float LayoutBox::clientHeight() const
  {
    float height = size().y;
    return height - borderTop() - borderBottom();
  }

  float LayoutBox::scrollWidth() const
  {
    if (isScrollContainer())
      return getScrollableArea()->scrollWidth();

    // TODO(yorkie): implement this.
    return 0;
  }

  float LayoutBox::scrollHeight() const
  {
    if (isScrollContainer())
      return getScrollableArea()->scrollHeight();

    // TODO(yorkie): implement this.
    return 0;
  }

  void LayoutBox::autoScroll(const glm::vec3 &offset)
  {
    // TODO(yorkie): implement this.
  }

  bool LayoutBox::scrollsOverflow() const
  {
    if (hasNonVisibleOverflow())
    {
      auto elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyle();
      string overflowXY = elementStyle.hasProperty("overflow")
                              ? elementStyle.getPropertyValue("overflow")
                              : "visible";
      return overflowXY == "auto" || overflowXY == "scroll";
    }
    else
    {
      return false;
    }
  }

  bool LayoutBox::scrollsOverflowX() const
  {
    if (hasNonVisibleOverflow())
    {
      auto elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyle();
      string overflowX = elementStyle.hasProperty("overflow-x")
                             ? elementStyle.getPropertyValue("overflow-x")
                             : "visible";
      return overflowX == "auto" || overflowX == "scroll";
    }
    else
    {
      return false;
    }
  }

  bool LayoutBox::scrollsOverflowY() const
  {
    if (hasNonVisibleOverflow())
    {
      auto elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyle();
      string overflowY = elementStyle.hasProperty("overflow-y")
                             ? elementStyle.getPropertyValue("overflow-y")
                             : "visible";
      return overflowY == "auto" || overflowY == "scroll";
    }
    else
    {
      return false;
    }
  }

  glm::vec3 LayoutBox::scrollOrigin() const
  {
    return getScrollableArea()
               ? getScrollableArea()->scrollOrigin()
               : glm::vec3(0, 0, 0);
  }

  glm::vec3 LayoutBox::scrolledContentOffset() const
  {
    assert(isScrollContainer());
    assert(getScrollableArea() != nullptr);
    return getScrollableArea()->getScrollOffset();
  }

  glm::vec3 LayoutBox::computeSize() const
  {
    Fragment nodeFragment = formattingContext().liveFragment();
    if (isText())
    {
      auto layoutText = dynamic_pointer_cast<const LayoutText>(shared_from_this());
      if (layoutText != nullptr && layoutText->plainTextLength() == 0)
        nodeFragment = Fragment::None(); // Set the fragment to none if a text and empty content.
    }
    return nodeFragment.size();
  }

  void LayoutBox::invalidateCachedGeometry()
  {
    setHasValidCachedGeometry(false);
    // TODO(yorkie): invalidate the cached geometry of the parent.
  }
}
