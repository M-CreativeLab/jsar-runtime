#pragma once

#include <memory>
#include <array>
#include <ostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "../math3d/utils.hpp"
#include "../math3d/plane.hpp"
#include "./culling/bounding_box.hpp"
#include "./culling/bounding_sphere.hpp"

namespace collision
{
  class TrRay
  {
  public:
    TrRay() : origin(glm::vec3{}), direction(glm::vec3{}), length(0.f) {}
    TrRay(const glm::vec3 &origin, const glm::vec3 &direction, float length = 0.f)
        : origin(origin),
          direction(direction),
          length(length)
    {
    }
    TrRay(const TrRay &ray) = default;
    ~TrRay() = default;

  public:
    /**
     * Calculate the intercept of a ray on a given axis.
     * 
     * @param axis to check 'x' | 'y' | 'z'
     * @param offset from axis interception (i.e. an offset of 1y is intercepted above ground)
     * @returns a vector containing the coordinates where 'axis' is equal to zero (else offset), or null if there is no
     *          intercept.
     */
    std::optional<glm::vec3> intersectsAxis(const std::string &axis, float offset)
    {
      if (axis == "y")
      {
        const auto t = (origin.y - offset) / direction.y;
        if (t > 0.f)
          return std::nullopt;
        return glm::vec3(origin.x + direction.x * -t, offset, origin.z + direction.z * -t);
      }
      if (axis == "x")
      {
        const auto t = (origin.x - offset) / direction.x;
        if (t > 0.f)
          return std::nullopt;
        return glm::vec3(offset, origin.y + direction.y * -t, origin.z + direction.z * -t);
      }
      if (axis == "z")
      {
        const auto t = (origin.z - offset) / direction.z;
        if (t > 0.f)
          return std::nullopt;
        return glm::vec3(origin.x + direction.x * -t, origin.y + direction.y * -t, offset);
      }
      else
      {
        return std::nullopt;
      }
    }

    /**
     * Checks if the ray intersects a box This does not account for the ray length by design to improve perfs.
     *
     * @param minimum the minimum point of the box
     * @param maximum the maximum point of the box
     * @param intersectionTreshold extra extend to be added to the BoundingBox in all direction.
     * @returns if the box was hit.
     */
    bool intersectsBoxMinMax(const glm::vec3 &minimum, const glm::vec3 &maximum,
                             float intersectionTreshold = 0.f) const
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

    /**
     * Checks if the ray intersects a box This does not account for the ray lenght by design to improve perfs.
     *
     * @param box the bounding box to check
     * @param intersectionTreshold extra extend to be added to the BoundingBox in all direction.
     * @returns if the box was hit
     */
    inline bool intersectsBox(const culling::TrBoundingBox &box, float intersectionTreshold = 0.f) const
    {
      return intersectsBoxMinMax(box.minimum, box.maximum, intersectionTreshold);
    }

    /**
     * If the ray hits a sphere.
     *
     * @param sphere The bounding sphere to check against.
     * @param intersectionTreshold extra extend to be added to the BoundingSphere in all direction.
     * @returns `true` if the ray intersects the sphere, otherwise `false`.
     */
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

    /**
     * Checks if ray intersects a plane.
     *
     * @param plane The plane to check against.
     * @returns the distance away it was hit.
     */
    std::optional<float> intersectsPlane(const math3d::TrPlane &plane) const
    {
      auto distance = 0.f;
      auto result1 = glm::dot(plane.normal, direction);

      if (std::abs(result1) < 9.99999997475243e-7f)
      {
        return std::nullopt;
      }
      else
      {
        const auto result2 = glm::dot(plane.normal, origin);
        distance = (-plane.d - result2) / result1;
        if (distance < 0.f)
        {
          if (distance < -9.99999997475243e-7)
          {
            return std::nullopt;
          }
          else
          {
            return 0.f;
          }
        }

        return distance;
      }
    }

    void update(const glm::vec3 &origin, const glm::vec3 &direction, float length = 1.f)
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

  public:
    friend std::ostream &operator<<(std::ostream &os, const TrRay &ray)
    {
      os << "Ray("
         << "origin=[" << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z << "], "
         << "dir=[" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << "], "
         << "length=" << ray.length
         << ")";
      return os;
    }
  };
}
