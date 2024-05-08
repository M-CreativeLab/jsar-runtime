#pragma once

#include <cmath>
#include <array>
#include <glm/glm.hpp>

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

  /**
   * Create a 4x4 matrix from an array of 16 floats.
   */
  glm::mat4 createMat4FromArray(float *src);
  glm::mat4 getOriginMatrix();
  glm::mat4 getProjectionMatrixInLH(glm::mat4 &src);
  glm::mat4 getViewMatrixInLH(glm::mat4 &src);
  glm::mat4 getViewMatrixWithTransform(glm::mat4 &worldToView, glm::mat4 &transform);
}
