#pragma once

#include <ostream>
#include <glm/glm.hpp>
#include "./fragment.hpp"

namespace client_layout
{
  class ConstraintSpace
  {
  public:
    ConstraintSpace(float width, float height)
        : size_(width, height)
    {
    }
    ConstraintSpace(const Fragment &fragment)
        : size_(fragment.width(), fragment.height())
    {
    }

  public:
    float width() const { return size_.x; }
    float height() const { return size_.y; }

  public:
    friend std::ostream &operator<<(std::ostream &os, const ConstraintSpace &space)
    {
      os << "ConstraintSpace(" << space.width() << ", " << space.height() << ")";
      return os;
    }

  private:
    glm::vec2 size_;
  };
}
