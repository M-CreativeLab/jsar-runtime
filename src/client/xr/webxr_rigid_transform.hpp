#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace client_xr
{
  /**
   * The `XRRigidTransform` is a WebXR API interface that represents the 3D geometric transform described by a position and
   * orientation.
   *
   * `XRRigidTransform` is used to specify transforms throughout the WebXR APIs, including:
   * - The offset and orientation relative to the parent reference space to use when creating a new reference space with
   *   `getOffsetReferenceSpace()`.
   * - The transform of an `XRView`.
   * - The transform of an `XRPose`.
   * - The `XRReferenceSpaceEvent` event's transform property, as found in the reset event received by an `XRReferenceSpace`.
   *
   * Using `XRRigidTransform` in these places rather than bare arrays that provide the matrix data has an advantage. It
   * automatically computes the inverse of the transform and even caches it making subsequent requests significantly faster.
   * 
   * @see https://developer.mozilla.org/en-US/docs/Web/API/XRRigidTransform
   */
  class XRRigidTransform
  {
  public:
    XRRigidTransform()
        : position_(glm::vec3(0.0f)),
          orientation_(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
          matrix_(glm::mat4(1.0f))
    {
    }
    XRRigidTransform(glm::vec3 position, glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
        : position_(position),
          orientation_(orientation),
          matrix_(glm::mat4_cast(orientation))
    {
      matrix_[3] = glm::vec4(position, 1.0f);
    }
    XRRigidTransform(glm::mat4 matrix)
        : matrix_(matrix)
    {
      position_ = glm::vec3(matrix[3]);
      orientation_ = glm::quat_cast(matrix);
    }

  public:
    /**
     * The read-only `inverse` property of the `XRRigidTransform` interface returns another `XRRigidTransform` object which
     * is the inverse of its owning transform.
     *
     * @returns an `XRRigidTransform` which contains the inverse of the `XRRigidTransform` on which it's accessed.
     */
    XRRigidTransform inverse()
    {
      glm::mat4 inverseMatrix = glm::inverse(matrix_);
      return XRRigidTransform(inverseMatrix);
    }
    /**
     * The read-only `XRRigidTransform` property matrix returns the transform matrix represented by the object. The returned
     * matrix can then be premultiplied with a column vector to rotate the vector by the 3D rotation specified by the orientation,
     * then translate it by the position.
     *
     * @returns a `glm::mat4` which represents the 4x4 transform matrix described by the position and orientation properties.
     */
    glm::mat4 &matrix() { return matrix_; }
    /**
     * The read-only `XRRigidTransform` property orientation is a `glm::quat` containing a normalized quaternion (also called a
     * unit quaternion or versor) specifying the rotational component of the transform represented by the object. If you specify
     * a quaternion whose length is not exactly 1.0 meters, it will be normalized for you.
     *
     * @returns a `glm::quat` object which contains a unit quaternion providing the orientation component of the transform. As a
     *          unit quaternion, the length of the returned quaternion is always 1.0 meters.
     */
    glm::quat &orientation() { return orientation_; }
    /**
     * The read-only `XRRigidTransform` property position is a `glm::vec3` object which provides the 3D point, specified in meters,
     * describing the translation component of the transform.
     *
     * @returns A `glm::vec3` indicating the 3D position component of the transform matrix. The units are meters.
     */
    glm::vec3 &position() { return position_; }

  private:
    glm::vec3 position_;
    glm::quat orientation_;
    glm::mat4 matrix_;
  };
}
