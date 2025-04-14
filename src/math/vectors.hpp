#pragma once

#include <cmath>
#include <ostream>
#include <glm/glm.hpp>

namespace math
{
  template <typename T>
  inline T signum(T value)
  {
    return std::copysign(1, value);
  }

  class Vec3 : public glm::vec3
  {
    using glm::vec3::vec3;

  public:
    static const Vec3 Identity() { return Vec3(0.0f, 0.0f, 0.0f); }
    static const Vec3 One() { return Vec3(1.0f, 1.0f, 1.0f); }

  public:
    Vec3(const glm::vec3 &vec)
        : glm::vec3(vec)
    {
    }

  public:
    /**
     * Appends the vector to the given data.
     *
     * @param data The data to append to.
     */
    void appendTo(std::vector<float> &data) const
    {
      data.push_back(x);
      data.push_back(y);
      data.push_back(z);
    }
    /**
     * It returns some vector that is orthogonal to the given one.
     *
     * @returns The orthogonal vector.
     */
    Vec3 anyOrthogonalVector()
    {
      if (std::abs(x) > std::abs(y))
        return Vec3(-z, 0.0f, x);
      else
        return Vec3(0.0f, z, -y);
    }
    /**
     * It returns any unit vector that is orthogonal to the given one.
     *
     * @returns The orthogonal vector.
     */
    Vec3 anyOrthonormalVector() const
    {
      assert(isNormalized());
      auto sign = signum(z);
      auto a = -1.0f / (sign + z);
      auto b = x * y * a;
      return Vec3(b, sign + y * y * a, -y);
    }
    /**
     * @returns The length of the vector.
     */
    inline float length() const
    {
      return glm::length(static_cast<glm::vec3>(*this));
    }
    /**
     * @returns The normalized vector.
     */
    inline Vec3 normalize() const
    {
      auto norm = glm::normalize(static_cast<glm::vec3>(*this));
      return Vec3(norm.x, norm.y, norm.z);
    }
    /**
     * @returns If the vector is normalized.
     */
    inline bool isNormalized() const
    {
      return std::fabs(length() - 1.0f) < std::numeric_limits<float>::epsilon();
    }
    /**
     * Computes the dot product of `this` and `rhs`.
     *
     * @param rhs The other vector.
     * @returns The dot product.
     */
    inline float dot(const Vec3 &rhs) const
    {
      return glm::dot(static_cast<glm::vec3>(*this), static_cast<glm::vec3>(rhs));
    }
    /**
     * Computes the cross product of `this` and `rhs`.
     *
     * @param rhs The other vector.
     * @returns The cross product.
     */
    inline Vec3 cross(const Vec3 &rhs) const
    {
      auto cross = glm::cross(static_cast<glm::vec3>(*this), static_cast<glm::vec3>(rhs));
      return Vec3(cross.x, cross.y, cross.z);
    }

  public:
    friend std::ostream &operator<<(std::ostream &os, const Vec3 &vec)
    {
      os << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
      return os;
    }
  };

  /**
   * The forward direction.
   */
  class Dir3 final : public Vec3
  {
    using Vec3::Vec3;

  public:
    /**
     * @returns The forward direction.
     */
    static Dir3 Up() { return Dir3(0.0f, 1.0f, 0.0f); }
    /**
     * @returns The forward direction.
     */
    static Dir3 Right() { return Dir3(1.0f, 0.0f, 0.0f); }
    /**
     * @returns The +Y direction.
     */
    static inline Dir3 Y() { return Up(); }
    /**
     * @returns The +X direction.
     */
    static inline Dir3 X() { return Right(); }
  };

  /**
   * The size in 3D.
   */
  class Size3 final : public Vec3
  {
    using Vec3::Vec3;

  public:
    size_t width() const { return x; }
    size_t height() const { return y; }
    size_t depth() const { return z; }
  };
}
