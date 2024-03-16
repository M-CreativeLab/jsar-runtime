#pragma once

#include <cmath>
#include <array>
#include "debug.hpp"
#include "vector.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace math
{
  /**
   * Convert the transformation or view matrix to right-handed coodinate system.
   */
  std::array<float, 16> ConvertMatrixToRightHanded(std::array<float, 16> &transform, int eyeId = -1);
}
