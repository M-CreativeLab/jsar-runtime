#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace math3d
{
  template <class T>
  constexpr typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
  almost_equal(T x, T y, int ulp = 4)
  {
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x - y) < std::numeric_limits<T>::epsilon() * std::abs(x + y) * static_cast<T>(ulp)
           // unless the result is subnormal
           || std::abs(x - y) < std::numeric_limits<T>::min();
  }

  inline void fill_vec2(glm::vec2 &v, float value)
  {
    v.x = value;
    v.y = value;
  }

  inline void fill_vec3(glm::vec3 &v, float value)
  {
    v.x = value;
    v.y = value;
    v.z = value;
  }

  class TrVector3
  {
  public:
    static inline void TransformCoordinatesFromFloatsToRef(float x, float y, float z,
                                                           const glm::mat4 &transformation, glm::vec3 &result)
    {
      const auto &m = glm::value_ptr(transformation);
      const auto rx = x * m[0] + y * m[4] + z * m[8] + m[12];
      const auto ry = x * m[1] + y * m[5] + z * m[9] + m[13];
      const auto rz = x * m[2] + y * m[6] + z * m[10] + m[14];
      const auto rw = 1 / (x * m[3] + y * m[7] + z * m[11] + m[15]);
      result.x = rx * rw;
      result.y = ry * rw;
      result.z = rz * rw;
    }

    static inline void TransformCoordinatesToRef(const glm::vec3 &vector,
                                                 const glm::mat4 &transformation, glm::vec3 &result)
    {
      return TransformCoordinatesFromFloatsToRef(vector.x, vector.y, vector.z, transformation, result);
    }

    static inline glm::vec3 &MinimizeInPlace(const glm::vec3 &other, glm::vec3 &result)
    {
      return MinimizeInPlaceFromFloats(other.x, other.y, other.z, result);
    }

    static inline glm::vec3 &MinimizeInPlaceFromFloats(float x, float y, float z, glm::vec3 &result)
    {
      if (x < result.x)
        result.x = x;
      if (y < result.y)
        result.y = y;
      if (z < result.z)
        result.z = z;
      return result;
    }

    static inline glm::vec3 &MaximizeInPlace(const glm::vec3 &other, glm::vec3 &result)
    {
      return MaximizeInPlaceFromFloats(other.x, other.y, other.z, result);
    }

    static inline glm::vec3 &MaximizeInPlaceFromFloats(float x, float y, float z, glm::vec3 &result)
    {
      if (x > result.x)
        result.x = x;
      if (y > result.y)
        result.y = y;
      if (z > result.z)
        result.z = z;
      return result;
    }

    static inline void FromArrayToRef(const float *array, unsigned int offset, glm::vec3 &result)
    {
      result.x = array[offset];
      result.y = array[offset + 1];
      result.z = array[offset + 2];
    }
  };

  namespace matrix
  {
    inline bool is_identity(const glm::mat4 &m)
    {
      return m[0][0] == 1.f && m[0][1] == 0.f && m[0][2] == 0.f && m[0][3] == 0.f &&
             m[1][0] == 0.f && m[1][1] == 1.f && m[1][2] == 0.f && m[1][3] == 0.f &&
             m[2][0] == 0.f && m[2][1] == 0.f && m[2][2] == 1.f && m[2][3] == 0.f &&
             m[3][0] == 0.f && m[3][1] == 0.f && m[3][2] == 0.f && m[3][3] == 1.f;
    }
  }
}
