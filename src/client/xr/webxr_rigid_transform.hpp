#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace client_xr
{
  /**
   * @class XRRigidTransform
   * The `XRRigidTransform` class represents a 3D geometric transform described by a position and orientation.
   *
   * It is used to specify transforms throughout the WebXR APIs, including:
   * - The offset and orientation relative to the parent reference space.
   * - The transform of an `XRView`.
   * - The transform of an `XRPose`.
   * - The `XRReferenceSpaceEvent` event's transform property.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/API/XRRigidTransform
   */
  class XRRigidTransform
  {
  public:
    /**
     * Default constructor for `XRRigidTransform`.
     */
    XRRigidTransform() noexcept
        : position_(glm::vec3(0.0f))
        , orientation_(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
        , matrix_(glm::mat4(1.0f))
    {
    }

    /**
     * Constructs a new `XRRigidTransform` with the specified position and orientation.
     *
     * @param position The position of the transform.
     * @param orientation The orientation of the transform (default: identity quaternion).
     */
    XRRigidTransform(glm::vec3 position,
                     glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) noexcept
        : position_(position)
        , orientation_(orientation)
        , matrix_(glm::mat4_cast(orientation))
    {
      matrix_[3] = glm::vec4(position, 1.0f);
    }

    /**
     * Constructs a new `XRRigidTransform` from a transformation matrix.
     *
     * @param matrix The transformation matrix.
     */
    explicit XRRigidTransform(glm::mat4 matrix) noexcept
        : matrix_(matrix)
        , position_(glm::vec3(matrix[3]))
        , orientation_(glm::quat_cast(matrix))
    {
    }

  public:
    /**
     * Gets the inverse of this transform.
     *
     * @returns An `XRRigidTransform` representing the inverse of this transform.
     */
    XRRigidTransform inverse() const noexcept
    {
      return XRRigidTransform(glm::inverse(matrix_));
    }

    /**
     * Gets the transformation matrix of this transform.
     *
     * @returns A `glm::mat4` representing the transformation matrix.
     */
    const glm::mat4 &matrix() const noexcept
    {
      return matrix_;
    }

    /**
     * Gets the orientation of this transform.
     *
     * @returns A `glm::quat` representing the orientation.
     */
    const glm::quat &orientation() const noexcept
    {
      return orientation_;
    }

    /**
     * Gets the position of this transform.
     *
     * @returns A `glm::vec3` representing the position.
     */
    const glm::vec3 &position() const noexcept
    {
      return position_;
    }

  private:
    glm::vec3 position_;    // The position component of the transform
    glm::quat orientation_; // The orientation component of the transform
    glm::mat4 matrix_;      // The transformation matrix
  };
} // namespace client_xr
