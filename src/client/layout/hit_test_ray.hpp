#pragma once

#include <common/collision/ray.hpp>

namespace client_layout
{
  /**
   * @brief HitTestRay is used to perform hit testing in the layout view.
   *
   * The ray is defined in the local space of the layout view, which is a right-handed coordinate system with the
   * following axes:
   *
   * - X-axis: Right (+X)
   * - Y-axis: Up (-Y)
   * - Z-axis: Depth (+Z)
   */
  class HitTestRay : public collision::TrRay
  {
    using collision::TrRay::TrRay;

  public:
    HitTestRay(const collision::TrRay &ray);

  public:
    bool operator==(const HitTestRay &other) const;
    bool operator!=(const HitTestRay &other) const
    {
      return !(*this == other);
    }
  };
}
