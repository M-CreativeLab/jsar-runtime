#pragma once

#include <string>
#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Element : public ecs::Component
  {
  public:
    using ecs::Component::Component;

    Element(std::string name)
        : name(name)
    {
    }

  public:
    std::string name;
  };
}
