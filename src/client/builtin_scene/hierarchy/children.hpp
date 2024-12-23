#pragma once

#include <vector>
#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Children : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    std::vector<ecs::EntityId> &children() { return children_; }

  private:
    std::vector<ecs::EntityId> children_;
  };
}
