#include <client/dom/node.hpp>
#include <client/dom/element.hpp>
#include <common/collision/ray.hpp>

#include "./geometry/bounding_box.hpp"
#include "./layout_box.hpp"
#include "./layout_text.hpp"

namespace endor
{
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
      return false;
    }

    bool LayoutBox::mayIntersect(const HitTestResult &r, const HitTestRay &ray, const glm::vec3 &accumulatedOffset) const
    {
      vector<geometry::BoundingBox> overflowBoxes;
      if (hasHitTestableOverflow())
      {
        // TODO(yorkie): handle the hit test for the box with overflow.
      }
      else
        overflowBoxes = getHitTestableBoundingBoxes();

      if (overflowBoxes.size() > 0)
      {
        bool intersects = false;
        for (auto box : overflowBoxes)
        {
          box.move(accumulatedOffset);

          auto min = box.minimumWorld;
          auto max = box.maximumWorld;
          if (ray.intersectsBoxMinMax(min, max))
          {
            intersects = true;
            break;
          }
        }
        return intersects;
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

    void LayoutBox::didComputeLayout(const ConstraintSpace &availableSpace)
    {
      LayoutBoxModelObject::didComputeLayout(availableSpace);

      // Update the hit-testable bounding box.
      updateHitTestableBoundingBoxes();
    }

    void LayoutBox::didComputeLayoutOnce(const ConstraintSpace &availableSpace)
    {
      LayoutBoxModelObject::didComputeLayoutOnce(availableSpace);

      // Update the scrollable overflow from the layout results.
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
      auto children = virtualChildren();
      if (!children)
        return {};

      vector<std::shared_ptr<LayoutBox>> childBoxes;
      for (const auto &child : *children)
      {
        if (child->isBox())
          childBoxes.push_back(static_pointer_cast<LayoutBox>(child));
      }
      return childBoxes;
    }

    void LayoutBox::updateHitTestableBoundingBoxes()
    {
      hit_testable_bounding_boxes_.clear();

      geometry::BoundingBox mainBoundingBox = physicalBorderBoxRect();
      hit_testable_bounding_boxes_.push_back(mainBoundingBox);

      glm::vec3 mainMin = mainBoundingBox.minimumWorld;
      glm::vec3 mainMax = mainBoundingBox.maximumWorld;

      for (const auto &childBox : getChildBoxes())
      {
        if (!childBox->visible())
          continue;

        for (const auto &childBoundingBox : childBox->getHitTestableBoundingBoxes())
        {
          // Skip extending if the child box has no size.
          if (glm::all(glm::epsilonEqual(childBoundingBox.extendSizeWorld,
                                         glm::vec3(0.0f),
                                         glm::epsilon<float>())))
          {
            continue;
          }

          // Check if the child box is fully contained in the main bounding box.
          glm::vec3 childMin = childBoundingBox.minimumWorld;
          glm::vec3 childMax = childBoundingBox.maximumWorld;
          if (childMin.x >= mainMin.x && childMin.y >= mainMin.y && childMin.z >= mainMin.z &&
              childMax.x <= mainMax.x && childMax.y <= mainMax.y && childMax.z <= mainMax.z)
          {
            continue; // Child box is fully contained, skip adding it.
          }
          hit_testable_bounding_boxes_.push_back(childBoundingBox);
        }
      }

      assert(hit_testable_bounding_boxes_.size() > 0);
    }
  }
} // namespace endor
