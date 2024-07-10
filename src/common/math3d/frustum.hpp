#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "./plane.hpp"

namespace math3d
{
  class TrFrustum
  {
  public:
    /**
     * Gets the planes representing the frustum
     * @param transform matrix to be applied to the returned planes
     * @returns a new array of 6 Frustum planes computed by the given transformation matrix.
     */
    static std::array<TrPlane, 6> GetPlanes(const glm::mat4 &transform)
    {
      std::array<TrPlane, 6> frustumPlanes{
          {TrPlane(0.f, 0.f, 0.f, 0.f), TrPlane(0.f, 0.f, 0.f, 0.f),
           TrPlane(0.f, 0.f, 0.f, 0.f), TrPlane(0.f, 0.f, 0.f, 0.f),
           TrPlane(0.f, 0.f, 0.f, 0.f), TrPlane(0.f, 0.f, 0.f, 0.f)}};
      TrFrustum::GetPlanesToRef(transform, frustumPlanes);
      return frustumPlanes;
    }

    /**
     * @brief Gets the near frustum plane transformed by the transform matrix
     * @param transform transformation matrix to be applied to the resulting frustum plane
     * @param frustumPlane the resuling frustum plane
     */
    static void GetNearPlaneToRef(const glm::mat4 &transform, TrPlane &frustumPlane)
    {
      const auto &m = glm::value_ptr(transform);
      frustumPlane.normal.x = m[3] + m[2];
      frustumPlane.normal.y = m[7] + m[6];
      frustumPlane.normal.z = m[11] + m[10];
      frustumPlane.d = m[15] + m[14];
      frustumPlane.normalize();
    }

    /**
     * @brief Gets the far frustum plane transformed by the transform matrix.
     * @param transform transformation matrix to be applied to the resulting frustum plane
     * @param frustumPlane the resuling frustum plane
     */
    static void GetFarPlaneToRef(const glm::mat4 &transform, TrPlane &frustumPlane)
    {
      const auto &m = glm::value_ptr(transform);
      frustumPlane.normal.x = m[3] - m[2];
      frustumPlane.normal.y = m[7] - m[6];
      frustumPlane.normal.z = m[11] - m[10];
      frustumPlane.d = m[15] - m[14];
      frustumPlane.normalize();
    }

    /**
     * @brief Gets the left frustum plane transformed by the transform matrix.
     * @param transform transformation matrix to be applied to the resulting frustum plane
     * @param frustumPlane the resuling frustum plane
     */
    static void GetLeftPlaneToRef(const glm::mat4 &transform, TrPlane &frustumPlane)
    {
      const auto &m = glm::value_ptr(transform);
      frustumPlane.normal.x = m[3] + m[0];
      frustumPlane.normal.y = m[7] + m[4];
      frustumPlane.normal.z = m[11] + m[8];
      frustumPlane.d = m[15] + m[12];
      frustumPlane.normalize();
    }

    /**
     * @brief Gets the right frustum plane transformed by the transform matrix.
     * @param transform transformation matrix to be applied to the resulting frustum plane
     * @param frustumPlane the resuling frustum plane
     */
    static void GetRightPlaneToRef(const glm::mat4 &transform, TrPlane &frustumPlane)
    {
      const auto &m = glm::value_ptr(transform);
      frustumPlane.normal.x = m[3] - m[0];
      frustumPlane.normal.y = m[7] - m[4];
      frustumPlane.normal.z = m[11] - m[8];
      frustumPlane.d = m[15] - m[12];
      frustumPlane.normalize();
    }

    /**
     * @brief Gets the top frustum plane transformed by the transform matrix.
     * @param transform transformation matrix to be applied to the resulting frustum plane
     * @param frustumPlane the resuling frustum plane
     */
    static void GetTopPlaneToRef(const glm::mat4 &transform, TrPlane &frustumPlane)
    {
      const auto &m = glm::value_ptr(transform);
      frustumPlane.normal.x = m[3] - m[1];
      frustumPlane.normal.y = m[7] - m[5];
      frustumPlane.normal.z = m[11] - m[9];
      frustumPlane.d = m[15] - m[13];
      frustumPlane.normalize();
    }

    /**
     * @brief Gets the bottom frustum plane transformed by the transform matrix.
     * @param transform transformation matrix to be applied to the resulting frustum plane
     * @param frustumPlane the resuling frustum plane
     */
    static void GetBottomPlaneToRef(const glm::mat4 &transform, TrPlane &frustumPlane)
    {
      const auto &m = glm::value_ptr(transform);
      frustumPlane.normal.x = m[3] + m[1];
      frustumPlane.normal.y = m[7] + m[5];
      frustumPlane.normal.z = m[11] + m[9];
      frustumPlane.d = m[15] + m[13];
      frustumPlane.normalize();
    }

    /**
     * @brief Sets the given array "frustumPlanes" with the 6 Frustum planes computed by the given
     * transformation matrix.
     * @param transform transformation matrix to be applied to the resulting frustum planes
     * @param frustumPlanes the resuling frustum planes
     */
    static void GetPlanesToRef(const glm::mat4 &transform, std::array<TrPlane, 6> &frustumPlanes)
    {
      // Near
      TrFrustum::GetNearPlaneToRef(transform, frustumPlanes[0]);

      // Far
      TrFrustum::GetFarPlaneToRef(transform, frustumPlanes[1]);

      // Left
      TrFrustum::GetLeftPlaneToRef(transform, frustumPlanes[2]);

      // Right
      TrFrustum::GetRightPlaneToRef(transform, frustumPlanes[3]);

      // Top
      TrFrustum::GetTopPlaneToRef(transform, frustumPlanes[4]);

      // Bottom
      TrFrustum::GetBottomPlaneToRef(transform, frustumPlanes[5]);
    }
  };
}
