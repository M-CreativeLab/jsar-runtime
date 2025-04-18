#pragma once

#include <iostream>
#include <glm/glm.hpp>

namespace client_layout::geometry
{
  /**
   * The viewport extents to 3 dimensions. It's used to restrict the objects to be visible in a viewport describing 
   * bounding box.
   */
  class Viewport3d : glm::uvec3
  {
  public:
    Viewport3d(float width, float height, float depth)
        : glm::uvec3(width, height, depth)
    {
    }

  public:
    inline uint32_t width() const { return x; }
    inline uint32_t height() const { return y; }
    inline uint32_t depth() const { return z; }

    inline uint32_t left() const { return 0; }
    inline uint32_t right() const { return left() + width(); }
    inline uint32_t top() const { return 0; }
    inline uint32_t bottom() const { return top() + height(); }
    inline uint32_t front() const { return depth() / 2; }
    inline uint32_t back() const { return -front(); }

    friend std::ostream &operator<<(std::ostream &os, const Viewport3d &viewport)
    {
      os << "Viewport3d(" << viewport.x << ", " << viewport.y << ", " << viewport.z << ")";
      return os;
    }
  };
}
