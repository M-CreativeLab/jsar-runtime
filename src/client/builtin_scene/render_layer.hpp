#pragma once

#include "./ecs.hpp"

namespace builtin_scene
{
  class RenderLayer : ecs::Component
  {
    using ecs::Component::Component;

  public:
    RenderLayer(int index = 0)
        : ecs::Component()
        , index_(index)
    {
    }
  
  private:
    int index_;
  };
}
