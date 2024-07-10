#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "common/math3d/utils.hpp"
#include "common/math3d/plane.hpp"
#include "./icullable.hpp"

namespace collision
{
  namespace culling
  {
    class TrBoundingBox : public TrICullable
    {
    public:
      TrBoundingBox(const glm::vec3 min, const glm::vec3 &max, const glm::mat4 &worldMatrix)
          : vectors{{glm::vec3{}, glm::vec3{}, glm::vec3{}, glm::vec3{},
                     glm::vec3{}, glm::vec3{}, glm::vec3{}, glm::vec3{}}},
            center{glm::vec3{}},
            centerWorld{glm::vec3{}},
            extendSize{glm::vec3{}},
            extendSizeWorld{glm::vec3{}},
            directions{{glm::vec3{}, glm::vec3{}, glm::vec3{}}},
            vectorsWorld{{glm::vec3{}, glm::vec3{}, glm::vec3{}, glm::vec3{},
                          glm::vec3{}, glm::vec3{}, glm::vec3{}, glm::vec3{}}},
            minimum{glm::vec3{}},
            minimumWorld{glm::vec3{}},
            maximum{glm::vec3{}},
            maximumWorld{glm::vec3{}},
            worldBaseMatrix{glm::mat4(1.0f)}
      {
        reConstruct(min, max, worldMatrix);
      }
      ~TrBoundingBox() override = default;

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
        vectors[0] = glm::vec3(minX, minY, minZ);
        vectors[1] = glm::vec3(maxX, minY, minZ);
        vectors[2] = glm::vec3(minX, maxY, minZ);
        vectors[3] = glm::vec3(maxX, maxY, minZ);
        vectors[4] = glm::vec3(minX, minY, maxZ);
        vectors[5] = glm::vec3(maxX, minY, maxZ);
        vectors[6] = glm::vec3(minX, maxY, maxZ);
        vectors[7] = glm::vec3(maxX, maxY, maxZ);

        // Obb
        center = (minimum + maximum) * 0.5f;
        extendSize = maximum - center;

        // World
        worldBaseMatrix = worldMatrix;
        _update(worldBaseMatrix);
      }
      TrBoundingBox &scale(float factor)
      {
        auto diff = maximum - minimum;
        const auto len = glm::length(diff);
        diff = glm::normalize(diff);

        const auto distance = len * factor;
        const auto newRadius = diff * (distance * 0.5f);

        auto min = center - newRadius;
        auto max = center + newRadius;
        reConstruct(min, max, worldBaseMatrix);
        return *this;
      }
      glm::mat4 &getWorldMatrix()
      {
        return worldBaseMatrix;
      }

      void _update(const glm::mat4 &worldMatrix)
      {
        auto &minWorld = minimumWorld;
        auto &maxWorld = maximumWorld;

        auto &_worldBaseMatrix = worldMatrix;
        fprintf(stdout, "worldbase matrix: (%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f)",
                _worldBaseMatrix[0][0], _worldBaseMatrix[0][1], _worldBaseMatrix[0][2], _worldBaseMatrix[0][3],
                _worldBaseMatrix[1][0], _worldBaseMatrix[1][1], _worldBaseMatrix[1][2], _worldBaseMatrix[1][3],
                _worldBaseMatrix[2][0], _worldBaseMatrix[2][1], _worldBaseMatrix[2][2], _worldBaseMatrix[2][3],
                _worldBaseMatrix[3][0], _worldBaseMatrix[3][1], _worldBaseMatrix[3][2], _worldBaseMatrix[3][3]);

        if (!math3d::matrix::is_identity(_worldBaseMatrix))
        {
          math3d::fill_vec3(minWorld, std::numeric_limits<float>::max());
          math3d::fill_vec3(maxWorld, std::numeric_limits<float>::lowest());

          for (auto index = 0u; index < 8; ++index)
          {
            auto &v = vectorsWorld[index];
            math3d::TrVector3::TransformCoordinatesToRef(vectors[index], _worldBaseMatrix, v);
            math3d::TrVector3::MinimizeInPlace(v, minWorld);
            math3d::TrVector3::MaximizeInPlace(v, maxWorld);
          }

          extendSizeWorld = (maxWorld - minWorld) * 0.5f;
          centerWorld = (maxWorld + minWorld) * 0.5f;
        }
        else
        {
          minWorld = minimum;
          maxWorld = maximum;
          for (auto index = 0u; index < 8; ++index)
            vectorsWorld[index] = vectors[index];

          // Extend
          extendSizeWorld = extendSize;
          centerWorld = center;
        }

        auto m = glm::value_ptr(_worldBaseMatrix);
        directions[0] = glm::vec3(m[0], m[1], m[2]);
        directions[1] = glm::vec3(m[4], m[5], m[6]);
        directions[2] = glm::vec3(m[8], m[9], m[10]);
      }

    public:
      bool isInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes, unsigned int strategy = 0) override
      {
        return TrBoundingBox::IsInFrustum(vectorsWorld, frustumPlanes);
      }
      bool isCompletelyInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes) override
      {
        return TrBoundingBox::IsCompletelyInFrustum(vectorsWorld, frustumPlanes);
      }

    private:
      static bool IsInFrustum(const std::array<glm::vec3, 8> &boundingVectors,
                              const std::array<math3d::TrPlane, 6> &frustumPlanes)
      {
        for (unsigned p = 0u; p < 6; ++p)
        {
          auto canReturnFalse = true;
          const auto &frustumPlane = frustumPlanes[p];
          for (unsigned i = 0u; i < 8; ++i)
          {
            if (frustumPlane.dotCoordinate(boundingVectors[i]) >= 0)
            {
              canReturnFalse = false;
              break;
            }
          }
          if (canReturnFalse)
          {
            return false;
          }
        }
        return true;
      }
      static bool IsCompletelyInFrustum(
          const std::array<glm::vec3, 8> &boundingVectors,
          const std::array<math3d::TrPlane, 6> &frustumPlanes)
      {
        for (unsigned p = 0u; p < 6; ++p)
        {
          const auto &frustumPlane = frustumPlanes[p];
          for (unsigned i = 0u; i < 8; ++i)
          {
            if (frustumPlane.dotCoordinate(boundingVectors[i]) < 0.f)
              return false;
          }
        }
        return true;
      }

    public:
      std::array<glm::vec3, 8> vectors;
      glm::vec3 center;
      glm::vec3 centerWorld;
      glm::vec3 extendSize;
      glm::vec3 extendSizeWorld;
      std::array<glm::vec3, 3> directions;
      std::array<glm::vec3, 8> vectorsWorld;
      glm::vec3 minimum;
      glm::vec3 minimumWorld;
      glm::vec3 maximum;
      glm::vec3 maximumWorld;

    private:
      glm::mat4 worldBaseMatrix;
    };
  }
}
