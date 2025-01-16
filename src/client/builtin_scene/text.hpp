#pragma once

#include <string>
#include "./ecs.hpp"

namespace builtin_scene
{
  class Text2d : public ecs::Component
  {
  public:
    Text2d(std::string content) : content(content) {}

  public:
    std::string content;
  };
}
