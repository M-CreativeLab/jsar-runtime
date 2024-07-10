#pragma once

#include <memory>
#include <array>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "common/math3d/utils.hpp"
#include "./culling/bounding_box.hpp"
#include "./culling/bounding_sphere.hpp"

namespace collision
{
  class TrRay
  {
  public:
    TrRay() : origin(glm::vec3{}), direction(glm::vec3{}), length(0.f) {}
    TrRay(const glm::vec3 &origin, const glm::vec3 &direction, float length = 0.f)
        : origin(origin), direction(direction), length(length) {}
    ~TrRay() = default;

  public:
    bool intersectsBoxMinMax(const glm::vec3 &minimum, const glm::vec3 &maximum, float intersectionTreshold = 0.f) const
    {
      const auto newMinimum = glm::vec3(minimum.x - intersectionTreshold,
                                        minimum.y - intersectionTreshold,
                                        minimum.z - intersectionTreshold);
      const auto newMaximum = glm::vec3(maximum.x + intersectionTreshold,
                                        maximum.y + intersectionTreshold,
                                        maximum.z + intersectionTreshold);
      auto d = 0.f;
      auto maxValue = std::numeric_limits<float>::max();
      auto inv = 0.f;
      auto min = 0.f;
      auto max = 0.f;
      if (std::abs(direction.x) < 0.0000001f)
      {
        if (origin.x < newMinimum.x || origin.x > newMaximum.x)
          return false;
      }
      else
      {
        inv = 1.f / direction.x;
        min = (newMinimum.x - origin.x) * inv;
        max = (newMaximum.x - origin.x) * inv;
        if (math3d::almost_equal(max, -std::numeric_limits<float>::infinity()))
          max = std::numeric_limits<float>::infinity();

        if (min > max)
          std::swap(min, max);

        d = std::max(min, d);
        maxValue = std::min(max, maxValue);

        if (d > maxValue)
          return false;
      }

      if (std::abs(direction.y) < 0.0000001f)
      {
        if (origin.y < newMinimum.y || origin.y > newMaximum.y)
          return false;
      }
      else
      {
        inv = 1.f / direction.y;
        min = (newMinimum.y - origin.y) * inv;
        max = (newMaximum.y - origin.y) * inv;

        if (math3d::almost_equal(max, -std::numeric_limits<float>::infinity()))
          max = std::numeric_limits<float>::infinity();

        if (min > max)
          std::swap(min, max);

        d = std::max(min, d);
        maxValue = std::min(max, maxValue);

        if (d > maxValue)
          return false;
      }

      if (std::abs(direction.z) < 0.0000001f)
      {
        if (origin.z < newMinimum.z || origin.z > newMaximum.z)
          return false;
      }
      else
      {
        inv = 1.f / direction.z;
        min = (newMinimum.z - origin.z) * inv;
        max = (newMaximum.z - origin.z) * inv;

        if (math3d::almost_equal(max, -std::numeric_limits<float>::infinity()))
          max = std::numeric_limits<float>::infinity();

        if (min > max)
          std::swap(min, max);

        d = std::max(min, d);
        maxValue = std::min(max, maxValue);

        if (d > maxValue)
          return false;
      }
      return true;
    }
    bool intersectsBox(const culling::TrBoundingBox &box, float intersectionTreshold = 0.f) const
    {
      return intersectsBoxMinMax(box.minimum, box.maximum, intersectionTreshold);
    }
    bool intersectsSphere(const culling::TrBoundingSphere &sphere, float intersectionTreshold) const
    {
      const auto x = sphere.center.x - origin.x;
      const auto y = sphere.center.y - origin.y;
      const auto z = sphere.center.z - origin.z;
      const auto pyth = x * x + y * y + z * z;
      const auto radius = sphere.radius + intersectionTreshold;
      const auto rr = radius * radius;

      if (pyth <= rr)
        return true;

      const auto dot = x * direction.x + y * direction.y + z * direction.z;
      if (dot < 0.f)
        return false;

      const auto temp = pyth - dot * dot;
      return temp <= rr;
    }
    void update(const glm::vec3& origin, const glm::vec3& direction, float length = 1.f)
    {
      this->origin = origin;
      this->direction = direction;
      if (length > 0.f)
        this->length = length;
    }

  public:
    glm::vec3 origin;
    glm::vec3 direction;
    float length;

  private:
    std::unique_ptr<TrRay> tmpRay = nullptr;
  };
}
