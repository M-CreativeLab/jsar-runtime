#pragma once

#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Parent : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    ecs::EntityId &parent() { return parent_; }

  private:
    ecs::EntityId parent_;
  };
}
