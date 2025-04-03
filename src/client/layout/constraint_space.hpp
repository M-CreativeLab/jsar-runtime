#pragma once

#include <glm/glm.hpp>

namespace client_layout
{
  class ConstraintSpace
  {
  public:
    ConstraintSpace(float width, float height)
        : size_(width, height)
    {
    }

  public:
    float width() const { return size_.x; }
    float height() const { return size_.y; }

  private:
    glm::vec2 size_;
  };
}
