#include <common/math_utils.hpp>
#include "./render_queue.hpp"

namespace builtin_scene
{
  using namespace transmute::common;

  bool RenderQueue::operator<(const RenderQueue &other) const
  {
    // Priority is determined by:
    //   1. translateZ: Physical depth in 3D space
    //   2. Effective zIndex: For positioned elements use zIndex, for non-positioned use 0.1 when zIndex=0
    //   3. base: Base queue number
    //
    if (!math_utils::ApproximatelyEqual(translateZ, other.translateZ))
      return translateZ < other.translateZ;

    // Calculate effective zIndex based on positioning
    // Magic number 0.1 only applies to non-positioned elements with zIndex=0
    float thisEffectiveZIndex = (isPositioned || zIndex != 0) ? static_cast<float>(zIndex) : 0.1f;
    float otherEffectiveZIndex = (other.isPositioned || other.zIndex != 0) ? static_cast<float>(other.zIndex) : 0.1f;

    if (!math_utils::ApproximatelyEqual(thisEffectiveZIndex, otherEffectiveZIndex))
      return thisEffectiveZIndex < otherEffectiveZIndex;

    return base < other.base;
  }

  bool RenderQueue::operator>(const RenderQueue &other) const
  {
    return !(*this < other) && !(*this == other);
  }

  bool RenderQueue::operator==(const RenderQueue &other) const
  {
    return base == other.base &&
           zIndex == other.zIndex &&
           math_utils::ApproximatelyEqual(translateZ, other.translateZ) &&
           isPositioned == other.isPositioned;
  }

  bool RenderQueue::operator!=(const RenderQueue &other) const
  {
    return !(*this == other);
  }
}
