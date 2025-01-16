#pragma once

#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Parent : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    Parent(ecs::EntityId parent) : parent_(parent) {}

  public:
    const ecs::EntityId &parent() const { return parent_; }

  private:
    ecs::EntityId parent_;
  };
}
