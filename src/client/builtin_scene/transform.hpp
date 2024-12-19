#pragma once

#include "./ecs.hpp"

namespace builtin_scene::transform
{
  class Position : public ecs::Component
  {
  public:
    Position() : x(0), y(0), z(0) {}
    Position(float x, float y, float z) : x(x), y(y), z(z) {}
    ~Position() = default;

  public:
    float x;
    float y;
    float z;
  };
}
