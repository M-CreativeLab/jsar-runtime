#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "./vectors.hpp"

namespace math
{
  class Quat : public glm::quat
  {
  public:
    static Quat Identity() { return Quat::FromXYZW(0, 0, 0, 1); }
    static Quat Zero() { return Quat(0, 0, 0, 0); }
    static Quat Nan() { return Quat(NAN, NAN, NAN, NAN); }

  public:
    /**
     * Creates a quaternion from the given components.
     *
     * @param x The x component.
     * @param y The y component.
     * @param z The z component.
     * @param w The w component.
     * @returns The quaternion.
     */
    static inline Quat FromXYZW(float x, float y, float z, float w)
    {
      return Quat(w, x, y, z);
    }
    /**
     * Creates a quaternion from the given quaternion.
     *
     * @param quat The quaternion.
     * @returns The quaternion.
     */
    static inline Quat From(const glm::quat &quat)
    {
      return Quat(quat.w, quat.x, quat.y, quat.z);
    }
    /**
     * Creates a quaternion from an axis and an angle.
     *
     * @param axis The axis of rotation.
     * @param angle The angle of rotation.
     * @returns The rotation quaternion.
     */
    static inline Quat FromAxisAngle(const Vec3 &axis, float angle)
    {
      return Quat::From(glm::angleAxis(angle, axis));
    }
    /**
     * Gets the minimal rotation for transforming `from` to `to`.  The rotation is in the plane spanned by the two vectors.  Will rotate
     * at most 180 degrees.
     *
     * @param from The source vector.
     * @param to The target vector.
     * @returns The rotation quaternion.
     */
    static Quat FromRotationArc(const Vec3 &from, const Vec3 &to)
    {
      auto fromNorm = from.normalize();
      auto toNorm = to.normalize();

      static const float ONE_MINUS_EPS = 1.0f - 2.0 * std::numeric_limits<float>::epsilon();
      float dot = fromNorm.dot(toNorm);
      if (dot > ONE_MINUS_EPS)
      {
        return Quat::Identity();
      }
      else if (dot < -ONE_MINUS_EPS)
      {
        return Quat::FromAxisAngle(from.anyOrthonormalVector(), glm::pi<float>());
      }
      else
      {
        auto c = fromNorm.cross(toNorm);
        return Quat::FromXYZW(c.x, c.y, c.z, 1.0f + dot).normalize();
      }
    }

  public:
    using glm::quat::quat;

  public:
    /**
     * @returns The normalized vector.
     */
    inline Quat normalize() const
    {
      return Quat::From(glm::normalize(static_cast<glm::quat>(*this)));
    }

  public: // custom equality operators
    inline bool operator==(const Quat &rhs) const
    {
      return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    inline bool operator!=(const Quat &rhs) const
    {
      return !(*this == rhs);
    }
  };
}
