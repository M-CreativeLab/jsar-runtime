#include "./LayerTypes.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace builtin_scene::layers
{
  glm::mat4 LayerNode::getAbsoluteTransform() const
  {
    glm::mat4 absoluteTransform = transform;

    // Apply scroll offset as translation
    absoluteTransform = glm::translate(absoluteTransform, -scrollOffset);

    // Accumulate parent transforms
    if (auto parentPtr = parent.lock())
    {
      absoluteTransform = parentPtr->getAbsoluteTransform() * absoluteTransform;
    }

    return absoluteTransform;
  }

  std::optional<SkRect> LayerNode::getAbsoluteClipRect() const
  {
    std::optional<SkRect> absoluteClip = clipRect;

    if (!absoluteClip.has_value())
      return std::nullopt;

    // Transform the clip rect by the accumulated transform
    glm::mat4 absoluteTransform = getAbsoluteTransform();

    // Apply transform to clip rect corners
    glm::vec4 topLeft(absoluteClip->fLeft, absoluteClip->fTop, 0.0f, 1.0f);
    glm::vec4 bottomRight(absoluteClip->fRight, absoluteClip->fBottom, 0.0f, 1.0f);

    topLeft = absoluteTransform * topLeft;
    bottomRight = absoluteTransform * bottomRight;

    SkRect transformedClip = SkRect::MakeLTRB(
      topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

    // Intersect with parent clip rects
    if (auto parentPtr = parent.lock())
    {
      auto parentClip = parentPtr->getAbsoluteClipRect();
      if (parentClip.has_value())
      {
        if (!transformedClip.intersect(parentClip.value()))
        {
          // No intersection - layer is completely clipped
          return SkRect::MakeEmpty();
        }
      }
    }

    return transformedClip;
  }

  bool LayerNode::isClippedByAncestor() const
  {
    if (auto parentPtr = parent.lock())
    {
      if (parentPtr->needsClipping || parentPtr->isClippedByAncestor())
        return true;
    }
    return false;
  }

  glm::vec3 LayerNode::getAccumulatedScrollOffset() const
  {
    glm::vec3 accumulated = scrollOffset;

    if (auto parentPtr = parent.lock())
    {
      accumulated += parentPtr->getAccumulatedScrollOffset();
    }

    return accumulated;
  }
}