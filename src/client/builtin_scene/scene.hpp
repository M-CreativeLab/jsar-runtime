#pragma once

#include <memory>
#include "./hierarchy.hpp"

namespace builtin_scene
{
  class Scene : public Hierarchy
  {
  public:
    Scene();
    ~Scene();

  public:
    void update();
  };
}
