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
    const std::vector<ecs::EntityId> &children() const { return children_; }
    std::vector<ecs::EntityId> &children() { return children_; }

  public:
    void addChild(ecs::EntityId child) { children_.push_back(child); }

  private:
    std::vector<ecs::EntityId> children_;
  };
}
