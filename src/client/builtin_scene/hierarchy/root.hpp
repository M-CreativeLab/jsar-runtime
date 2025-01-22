#pragma once

#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Root : public ecs::Component
  {
  public:
    Root(bool renderable = false) : renderable(renderable)
    {
    }

  public:
    // If the root entity can be renderer.
    bool renderable;
  };
}
