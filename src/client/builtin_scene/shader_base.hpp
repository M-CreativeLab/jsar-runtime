#pragma once

#include <string>

namespace builtin_scene
{
  class ShaderRef
  {
  public:
    ShaderRef(std::string name) : name(name)
    {
    }

  public:
    std::string name;
  };
}
