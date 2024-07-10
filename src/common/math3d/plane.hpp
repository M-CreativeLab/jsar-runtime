#pragma once

#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "./utils.hpp"

namespace math3d
{
  class TrPlane
  {
  public:
    TrPlane() = default;
    TrPlane(const glm::vec3 &normal, float d) : normal(normal), d(d) {}
    TrPlane(float a, float b, float c, float d) : normal(a, b, c), d(d) {}
    ~TrPlane() = default;

  public:
    TrPlane copy() const { return TrPlane(*this); }
    std::array<float, 4> asArray() const { return {{normal.x, normal.y, normal.z, d}}; }
    TrPlane &normalize()
    {
      const float norm = sqrtf((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
      auto magnitude = 0.f;

      if (!almost_equal(norm, 0.f))
        magnitude = 1.f / norm;
      normal.x = normal.x * magnitude;
      normal.y = normal.y * magnitude;
      normal.z = normal.z * magnitude;
      d *= magnitude;
      return *this;
    }
    [[nodiscard]] TrPlane transform(const glm::mat4 &transformation) const
    {
      auto invertedMatrix = glm::inverse(transformation);
      const auto &m = glm::value_ptr(invertedMatrix);
      const auto x = normal.x;
      const auto y = normal.y;
      const auto z = normal.z;

      const auto normalX = x * m[0] + y * m[1] + z * m[2] + d * m[3];
      const auto normalY = x * m[4] + y * m[5] + z * m[6] + d * m[7];
      const auto normalZ = x * m[8] + y * m[9] + z * m[10] + d * m[11];
      const auto finalD = x * m[12] + y * m[13] + z * m[14] + d * m[15];
      return TrPlane(normalX, normalY, normalZ, finalD);
    }
    [[nodiscard]] float dotCoordinate(const glm::vec3 &point) const
    {
      return ((((normal.x * point.x) + (normal.y * point.y)) + (normal.z * point.z)) + d);
    }

  public:
    glm::vec3 normal;
    float d;
  };
}
