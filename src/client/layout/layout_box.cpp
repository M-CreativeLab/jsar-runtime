#include <client/dom/node.hpp>
#include <client/dom/element.hpp>
#include <common/collision/ray.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/builtin_scene/instanced_mesh.hpp>
#include <client/builtin_scene/meshes.hpp>

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
      bool scrolled = scrollable_area->scrollTo(offset);
      if (scrolled)
      {
        updateScrollShadows(); // Update scroll shadows after scrolling
      }
      return scrolled;
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
      bool scrolled = scrollable_area->scrollBy(offset);
      if (scrolled)
      {
        updateScrollShadows(); // Update scroll shadows after scrolling
      }
      return scrolled;
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

  void LayoutBox::updateScrollShadows()
  {
    // Only update scroll shadows for scroll containers
    if (!isScrollContainer() || !hasEntity())
      return;

    auto scrollableArea = getScrollableArea();
    if (!scrollableArea)
      return;

    // Update scroll shadow properties on the associated instance
    auto updateInstance = [this, scrollableArea](builtin_scene::Scene &scene)
    {
      auto webContextCtx = scene.getResource<builtin_scene::WebContentContext>();
      if (!webContextCtx)
        return;

      auto &instancedMesh = scene.getComponentChecked<builtin_scene::Mesh3d>(webContextCtx->instancedMeshEntity())
                              .getHandleCheckedAsRef<builtin_scene::InstancedMeshBase>();

      try
      {
        auto &instance = instancedMesh.getInstance(entity());

        // Get current scroll state
        glm::vec3 scrollOffset = scrollableArea->getScrollOffset();
        glm::vec3 scrollOrigin = scrollableArea->scrollOrigin();
        float scrollWidth = scrollableArea->scrollWidth();
        float scrollHeight = scrollableArea->scrollHeight();

        // Set scroll offset (only x,y components for 2D scrolling)
        instance.setScrollOffset(glm::vec2(scrollOffset.x, scrollOffset.y));

        // Set content size (total scrollable dimensions)
        instance.setContentSize(glm::vec2(scrollWidth, scrollHeight));

        // Set default scroll shadow color (dark semi-transparent)
        instance.setScrollShadowColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.3f));

        // Set default max shadow height to 20% of element dimensions
        instance.setScrollShadowMaxHeight(0.2f);
      }
      catch (const std::exception &e)
      {
        // Instance might not exist yet, which is fine
        // This can happen during initial layout before rendering is set up
      }
    };

    useSceneWithCallback(updateInstance);
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
    optional<geometry::BoundingBox> overflowBox = nullopt;
    if (hasHitTestableOverflow())
    {
      // TODO(yorkie): handle the hit test for the box with overflow.
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

      // Update scroll shadows when layout is computed for scroll containers
      updateScrollShadows();
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
}
