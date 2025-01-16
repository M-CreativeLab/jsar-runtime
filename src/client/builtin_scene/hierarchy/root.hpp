#pragma once

#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Root : public ecs::Component
  {
  public:
    using ecs::Component::Component;
  };
}
