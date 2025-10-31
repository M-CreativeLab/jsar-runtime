#pragma once

#include <cmath>
#include <cassert>
#include <algorithm>

namespace transmute::common
{
  class math_utils
  {
  public:
    /**
     * Clamp a value between a minimum and maximum, commonly it's from 0.0f to 1.0f.
     * 
     * @param value The value to clamp.
     * @param min The minimum value, default is 0.0f.
     * @param max The maximum value, default is 1.0f.
     * @returns The clamped value, which is always between min and max.
     */
    static float Clamp(float value, float min = 0.0f, float max = 1.0f)
    {
      return std::max(min, std::min(value, max));
    }

    /**
     * Clamp a value to be non-negative, such as clamping all negative values to 0.0f.
     * 
     * @param value The value to clamp.
     * @returns The clamped value, which is always >= 0.0f.
     */
    static float ClampNegativeToZero(float value)
    {
      return std::max(0.0f, value);
    }

    /**
     * Check if two floating-point numbers are approximately equal within a small epsilon.
     * 
     * @param a The first number.
     * @param b The second number.
     * @param epsilon The tolerance for comparison, default is 1e-6.
     * @returns True if the numbers are approximately equal, false otherwise.
     */
    static bool ApproximatelyEqual(float a, float b, float epsilon = 1e-6)
    {
      return std::fabs(a - b) <= epsilon;
    }

    /**
     * Check if a number is a power of two.
     * 
     * @param n The number to check, must be non-zero.
     * @returns True if n is a power of two, false otherwise.
     */
    static bool IsPowerOfTwo(uint64_t n)
    {
      assert(n != 0);
      return (n & (n - 1)) == 0;
    }
  };

  inline bool IsPtrAligned(const void *ptr, size_t alignment)
  {
    assert(math_utils::IsPowerOfTwo(alignment));
    assert(alignment != 0);
    return (reinterpret_cast<size_t>(ptr) & (alignment - 1)) == 0;
  }

  inline bool IsAligned(uint32_t value, size_t alignment)
  {
    assert(alignment <= UINT32_MAX);
    assert(math_utils::IsPowerOfTwo(alignment));
    assert(alignment != 0);
    uint32_t alignment32 = static_cast<uint32_t>(alignment);
    return (value & (alignment32 - 1)) == 0;
  }

  template <typename T>
  inline T *AlignPtr(T *ptr, size_t alignment)
  {
    assert(math_utils::IsPowerOfTwo(alignment));
    assert(alignment != 0);
    return reinterpret_cast<T *>((reinterpret_cast<size_t>(ptr) + (alignment - 1)) &
                                 ~(alignment - 1));
  }

  template <typename T>
  inline const T *AlignPtr(const T *ptr, size_t alignment)
  {
    assert(math_utils::IsPowerOfTwo(alignment));
    assert(alignment != 0);
    return reinterpret_cast<const T *>((reinterpret_cast<size_t>(ptr) + (alignment - 1)) &
                                       ~(alignment - 1));
  }
}
