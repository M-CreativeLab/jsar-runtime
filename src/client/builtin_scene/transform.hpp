#pragma once

#include <glm/glm.hpp>
#include "./ecs.hpp"

namespace builtin_scene::transform
{
  class Position : public ecs::Component
  {
  public:
    Position() : value_(0, 0, 0)
    {
    }
    Position(float x, float y, float z) : value_(x, y, z)
    {
    }
    ~Position() = default;

  private:
    glm::vec3 value_;
  };
}
