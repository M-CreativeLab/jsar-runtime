#pragma once

#include <string>
#include "./ecs.hpp"

namespace builtin_scene
{
  class Text : public ecs::Component
  {
  public:
    Text(std::string content) : content(content) {}

  public:
    std::string content;
  };
}
