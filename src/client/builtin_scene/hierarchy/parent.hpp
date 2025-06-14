#pragma once

#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Parent : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    Parent(ecs::EntityId parent, ecs::EntityId root)
        : parent_(parent)
        , root_(root)
    {
    }

  public:
    /**
     * @returns The parent entity ID.
     */
    const ecs::EntityId &parent() const
    {
      return parent_;
    }
    /**
     * @returns The root entity ID.
     */
    const ecs::EntityId &root() const
    {
      return root_;
    }

  private:
    ecs::EntityId parent_;
    ecs::EntityId root_;
  };
}
