#pragma once

#include <array>
#include "common/math3d/plane.hpp"

namespace collision
{
  namespace culling
  {
    struct TrICullable
    {
      virtual ~TrICullable() = default;
      virtual bool isInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes, unsigned int strategy = 0) = 0;
      virtual bool isCompletelyInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes) = 0;
    };
  }
}
