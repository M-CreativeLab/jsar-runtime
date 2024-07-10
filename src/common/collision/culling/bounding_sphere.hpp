#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "common/math3d/plane.hpp"
#include "./icullable.hpp"

namespace collision
{
  namespace culling
  {
    class TrBoundingSphere : public TrICullable
    {
    public:
      TrBoundingSphere(const glm::vec3 min, const glm::vec3 &max, const glm::mat4 &worldMatrix)
          : center{glm::vec3{}},
            centerWorld{glm::vec3{}},
            radius{0.f},
            radiusWorld{0.f},
            minimum{glm::vec3{}},
            maximum{glm::vec3{}},
            worldBaseMatrix{glm::mat4{1.0}}
      {
        reConstruct(min, max, worldMatrix);
      }
      ~TrBoundingSphere() override = default;

    public:
      void reConstruct(const glm::vec3 min, const glm::vec3 &max, const glm::mat4 &worldMatrix)
      {
        const auto minX = min.x,
                   minY = min.y,
                   minZ = min.z,
                   maxX = max.x,
                   maxY = max.y,
                   maxZ = max.z;

        minimum = glm::vec3(minX, minY, minZ);
        maximum = glm::vec3(maxX, maxY, maxZ);

        auto distance = glm::distance(minimum, maximum);
        center = (minimum + maximum) * 0.5f;
        radius = distance * 0.5f;

        worldBaseMatrix = worldMatrix;
        _update(worldBaseMatrix);
      }
      TrBoundingSphere &scale(float factor)
      {
        const auto &newRadius = radius * factor;
        auto tmpRadiusVec = glm::vec3(newRadius);
        auto min = center - tmpRadiusVec;
        auto max = center + tmpRadiusVec;
        reConstruct(min, max, worldBaseMatrix);
        return *this;
      }
      glm::mat4 &getWorldMatrix()
      {
        return worldBaseMatrix;
      }

      void _update(const glm::mat4 &worldMatrix)
      {
        // TODO
      }

    public:
      bool isInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes, unsigned int strategy = 0) override
      {
        const auto &iCenter = centerWorld;
        const auto &iRadius = radiusWorld;
        for (unsigned i = 0u; i < 6; ++i)
        {
          if (frustumPlanes[i].dotCoordinate(iCenter) <= -iRadius)
          {
            return false;
          }
        }
        return true;
      }
      bool isCompletelyInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes) override
      {
        const auto &iCenter = centerWorld;
        for (unsigned i = 0u; i < 6; ++i)
        {
          if (frustumPlanes[i].dotCoordinate(iCenter) < 0.f)
          {
            return false;
          }
        }
        return true;
      }
      bool isCenterInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes) const
      {
        const auto &iCenter = centerWorld;
        for (unsigned i = 0u; i < 6; ++i)
        {
          if (frustumPlanes[i].dotCoordinate(iCenter) < 0.f)
          {
            return false;
          }
        }
        return true;
      }

    public:
      glm::vec3 center;
      glm::vec3 centerWorld;
      float radius;
      float radiusWorld;
      glm::vec3 minimum;
      glm::vec3 maximum;

    private:
      glm::mat4 worldBaseMatrix;
    };
  }
}
