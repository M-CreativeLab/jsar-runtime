#include <common/math_utils.hpp>
#include "./render_queue.hpp"

namespace builtin_scene
{
  using namespace transmute::common;

  bool RenderQueue::operator<(const RenderQueue &other) const
  {
    // Priority is determined by:
    //   1. translateZ: Physical depth in 3D space
    //   2. zIndex: Logical stacking order
    //   3. isPositioned: When zIndex=0, positioned elements render after non-positioned
    //   4. base: Base queue number
    //
    if (!math_utils::ApproximatelyEqual(translateZ, other.translateZ))
      return translateZ < other.translateZ;
    if (zIndex != other.zIndex)
      return zIndex < other.zIndex;

    // When zIndex is equal and 0, positioned elements should render after non-positioned ones
    if (zIndex == 0 && other.zIndex == 0 && isPositioned != other.isPositioned)
      return !isPositioned; // non-positioned (false) < positioned (true)

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
