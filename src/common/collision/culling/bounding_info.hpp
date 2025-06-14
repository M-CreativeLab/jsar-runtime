#pragma once

#include <array>
#include <glm/glm.hpp>
#include "common/math3d/plane.hpp"
#include "../constants.hpp"
#include "./icullable.hpp"
#include "./bounding_box.hpp"
#include "./bounding_sphere.hpp"

namespace collision
{
  namespace culling
  {
    using CollisionConstants = collision::Constants;

    class TrBoundingInfo : public TrICullable
    {
    public:
      TrBoundingInfo(const glm::vec3 min, const glm::vec3 &max, const glm::mat4 &worldMatrix)
          : boundingBox(min, max, worldMatrix)
          , boundingSphere(min, max, worldMatrix)
      {
      }
      ~TrBoundingInfo() override = default;

    public:
      void reConstruct(const glm::vec3 min, const glm::vec3 &max, const glm::mat4 &worldMatrix)
      {
        boundingBox.reConstruct(min, max, worldMatrix);
        boundingSphere.reConstruct(min, max, worldMatrix);
      }
      void update(const glm::mat4 &worldMatrix)
      {
        boundingBox._update(worldMatrix);
        boundingSphere._update(worldMatrix);
      }

    public:
      bool isInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes, unsigned int strategy = 0) override
      {
        const auto inclusionTest = (strategy == CollisionConstants::MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION ||
                                    strategy == CollisionConstants::MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION_THEN_BSPHERE_ONLY);
        if (inclusionTest)
        {
          if (boundingSphere.isCenterInFrustum(frustumPlanes))
            return true;
        }
        if (!boundingSphere.isInFrustum(frustumPlanes))
          return false;

        const auto bSphereOnlyTest = (strategy == Constants::MESHES_CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY ||
                                      strategy == Constants::MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION_THEN_BSPHERE_ONLY);
        if (bSphereOnlyTest)
          return true;
        return boundingBox.isInFrustum(frustumPlanes);
      }
      bool isCompletelyInFrustum(const std::array<math3d::TrPlane, 6> &frustumPlanes) override
      {
        return boundingBox.isCompletelyInFrustum(frustumPlanes);
      }

    public:
      TrBoundingBox boundingBox;
      TrBoundingSphere boundingSphere;
    };
  }
}
