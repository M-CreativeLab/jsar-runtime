#include <client/dom/node.hpp>
#include <client/dom/element.hpp>
#include <common/collision/ray.hpp>

#include "./geometry/bounding_box.hpp"
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

  math3d::TrPlane LayoutBox::physicalBorderBoxFront() const
  {
    auto transformComponent = getSceneComponent<builtin_scene::Transform>();
    auto latestMatrix = transformComponent->lastComputedMatrix();

    glm::vec4 originalNormal(0, 0, 1, 0);
    glm::vec4 originalPoint(0, 0, 0, 1);

    glm::vec3 normal = glm::normalize(glm::vec3(
      glm::inverse(glm::transpose(latestMatrix)) * originalNormal));

    glm::vec3 point = glm::vec3(latestMatrix * originalPoint);
    float distance = -glm::dot(normal, point);

    math3d::TrPlane plane(normal, distance);
    return plane;
  }

  geometry::BoundingBox LayoutBox::physicalBorderBoxRect() const
  {
    auto transformComponent = getSceneComponent<builtin_scene::Transform>();
    auto min = glm::vec3(-0.5, -0.5, -0.5);
    auto max = min * -1.0f;
    return geometry::BoundingBox(min, max, transformComponent->lastComputedMatrix());
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
    const auto &elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyleRef();
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
    const auto &elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyleRef();
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
    // TODO(yorkie): consider the scrollbars' width?
    return borderLeft();
  }

  float LayoutBox::clientTop() const
  {
    // TODO(yorkie): consider the scrollbars' width?
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
    // TODO(yorkie): implement the autoscroll
  }

  bool LayoutBox::scrollTo(const glm::vec3 &offset)
  {
    if (TR_UNLIKELY(!isScrollContainer()))
      return false;

    auto scrollable_area = getScrollableArea();
    // Performance optimization: check if scrolling is actually needed
    if (scrollable_area && scrollable_area->needsScrolling())
    {
      scrollable_area->scrollTo(offset);
      return true;
    }
    else
    {
      return false;
    }
  }

  bool LayoutBox::scrollBy(const glm::vec3 &offset)
  {
    if (TR_UNLIKELY(!isScrollContainer()))
      return false;

    // Performance optimization: early exit for zero offset
    if (offset.x == 0.0f && offset.y == 0.0f && offset.z == 0.0f)
      return false;

    auto scrollable_area = getScrollableArea();
    if (scrollable_area && scrollable_area->needsScrolling())
    {
      return scrollable_area->scrollBy(offset);
    }
    else
    {
      return false;
    }
  }

  bool LayoutBox::scrollsOverflow() const
  {
    if (hasNonVisibleOverflow())
    {
      const auto &elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyleRef();
      return elementStyle.overflowX().isAutoOrScroll() ||
             elementStyle.overflowY().isAutoOrScroll();
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
      const auto &elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyleRef();
      return elementStyle.overflowX().isAutoOrScroll();
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
      const auto &elementStyle = dom::Node::As<dom::Element>(node())->adoptedStyleRef();
      return elementStyle.overflowY().isAutoOrScroll();
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

  bool LayoutBox::nodeAtPoint(HitTestResult &r, const HitTestRay &ray, const glm::vec3 &accumulatedOffset, HitTestPhase phase)
  {
    if (!mayIntersect(r, ray, accumulatedOffset))
      return false;

    bool skipChildren = false;
    // TODO(yorkie): support set `skipChildren` from the hit test request.
    if (!skipChildren && hitTestChildren(r, ray, accumulatedOffset, phase))
      return true;

    auto hitDistance = ray.intersectsPlane(physicalBorderBoxFront());
    if (!hitDistance.has_value())
      return false;

    // TODO(yorkie): implement the more accurate hit test? Such as testing for the radius of the box, or other shapes.
    if (node()->isElement())
    {
      glm::vec3 hitPoint = ray.origin + ray.direction * hitDistance.value();
      r.setNodeAndPosition(node(), hitPoint);
    }
    return true;
  }

  bool LayoutBox::hasHitTestableOverflow() const
  {
    // We only consider hit-testable overflow when both axes are 'visible'.
    // `hasNonVisibleOverflow()` is set when either overflow-x or overflow-y is NOT visible (i.e. establishes a clip).
    // For overflow:visible we must allow descendants that paint (and therefore can be hit) outside the border box
    // per CSS 2.1 visual overflow / hit-testing semantics.
    if (hasNonVisibleOverflow())
      return false;

    // Fast path: if there are no element children, nothing can extend beyond us.
    auto selfBBox = physicalBorderBoxRect();
    const glm::vec3 selfMin = selfBBox.minimumWorld;
    const glm::vec3 selfMax = selfBBox.maximumWorld;

    for (const auto &childBox : getChildBoxes())
    {
      auto childBBox = childBox->physicalBorderBoxRect();
      const glm::vec3 cMin = childBBox.minimumWorld;
      const glm::vec3 cMax = childBBox.maximumWorld;

      // If any component lies outside our border box, we have hit-testable overflow.
      if (cMin.x < selfMin.x || cMin.y < selfMin.y || cMin.z < selfMin.z ||
          cMax.x > selfMax.x || cMax.y > selfMax.y || cMax.z > selfMax.z)
        return true;
    }
    return false;
  }

  bool LayoutBox::mayIntersect(const HitTestResult &r, const HitTestRay &ray, const glm::vec3 &accumulatedOffset) const
  {
    optional<geometry::BoundingBox> overflowBox = nullopt;
    if (hasHitTestableOverflow())
    {
      // overflow:visible => union of self + children (minimal implementation)
      auto selfBBox = physicalBorderBoxRect();
      glm::vec3 unionMin = selfBBox.minimumWorld;
      glm::vec3 unionMax = selfBBox.maximumWorld;

      for (const auto &childBox : getChildBoxes())
      {
        auto childBBox = childBox->physicalBorderBoxRect();
        unionMin.x = std::min(unionMin.x, childBBox.minimumWorld.x);
        unionMin.y = std::min(unionMin.y, childBBox.minimumWorld.y);
        unionMin.z = std::min(unionMin.z, childBBox.minimumWorld.z);
        unionMax.x = std::max(unionMax.x, childBBox.maximumWorld.x);
        unionMax.y = std::max(unionMax.y, childBBox.maximumWorld.y);
        unionMax.z = std::max(unionMax.z, childBBox.maximumWorld.z);
      }

      // Apply accumulated offset (same semantics as clipped path below)
      unionMin += accumulatedOffset;
      unionMax += accumulatedOffset;
      return ray.intersectsBoxMinMax(unionMin, unionMax);
    }
    else
    {
      overflowBox = physicalBorderBoxRect();
    }

    if (overflowBox.has_value())
    {
      overflowBox->move(accumulatedOffset);

      auto min = overflowBox->minimumWorld;
      auto max = overflowBox->maximumWorld;
      return ray.intersectsBoxMinMax(min, max);
    }
    else
    {
      return false;
    }
  }

  bool LayoutBox::hitTestChildren(HitTestResult &r, const HitTestRay &ray, const glm::vec3 &accumulatedOffset, HitTestPhase phase)
  {
    for (auto child = slowLastChild(); child;
         child = child->prevSibling())
    {
      if (child->isText()) // Text nodes are not hit-testable.
        continue;

      glm::vec3 childAccumulatedOffset = accumulatedOffset;
      if (child->isBox())
      {
        // TODO(yorkie): append the child offset
      }

      if (child->nodeAtPoint(r, ray, childAccumulatedOffset, phase))
        return true;
    }
    return false;
  }

  void LayoutBox::didComputeLayoutOnce(const ConstraintSpace &availableSpace)
  {
    LayoutBoxModelObject::didComputeLayoutOnce(availableSpace);

    setScrollableOverflowFromLayoutResults();
    if (isScrollContainer())
    {
      getScrollableArea()
        ->updateAfterLayout(formattingContext().liveFragment());
    }
  }

  void LayoutBox::updateFromStyle()
  {
    LayoutBoxModelObject::updateFromStyle();

    auto m_style = style();
    if (!m_style.has_value())
      return;

    setHasNonVisibleOverflow(!m_style->overflowX().isVisible() || !m_style->overflowY().isVisible());
  }

  bool LayoutBox::isHorizontalWritingMode() const
  {
    auto nodeStyle = styleRef();
    string writingMode = nodeStyle.hasProperty("writing-mode")
                           ? nodeStyle.getPropertyValue("writing-mode")
                           : "horizontal-tb";
    return writingMode == "horizontal-tb" || writingMode == "sideways-lr";
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
    return nodeFragment.contentSize();
  }

  void LayoutBox::invalidateCachedGeometry()
  {
    setHasValidCachedGeometry(false);
    // TODO(yorkie): invalidate the cached geometry of the parent.
  }

  vector<shared_ptr<LayoutBox>> LayoutBox::getChildBoxes() const
  {
    vector<shared_ptr<LayoutBox>> childBoxes;
    for (auto child = slowFirstChild(); child; child = child->nextSibling())
    {
      if (!child->isBox())
        continue;
      auto childBox = static_pointer_cast<LayoutBox>(child);
      if (childBox)
        childBoxes.push_back(childBox);
    }
    return childBoxes;
  }
}
