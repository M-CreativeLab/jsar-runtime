#pragma once

#include <ostream>
#include <glm/glm.hpp>
#include "./fragment.hpp"

namespace client_layout
{
  class ConstraintSpace
  {
  public:
    // Zero space: 0 x 0
    static ConstraintSpace Zero()
    {
      return ConstraintSpace(0, 0);
    }

  public:
    ConstraintSpace(float width, float height)
        : size_(width, height)
    {
    }
    ConstraintSpace(const Fragment &fragment)
        : size_(fragment.contentWidth(), fragment.contentHeight())
    {
    }

  public:
    float width() const
    {
      return size_.x;
    }
    float height() const
    {
      return size_.y;
    }

  public:
    friend std::ostream &operator<<(std::ostream &os, const ConstraintSpace &space)
    {
      os << "ConstraintSpace(" << space.width() << ", " << space.height() << ")";
      return os;
    }
    bool operator==(const ConstraintSpace &other) const
    {
      return size_ == other.size_;
    }
    bool operator!=(const ConstraintSpace &other) const
    {
      return !(*this == other);
    }

  private:
    glm::vec2 size_;
  };
}
