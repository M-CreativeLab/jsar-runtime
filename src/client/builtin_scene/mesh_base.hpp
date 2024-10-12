#pragma once

#include <memory>
#include "./renderable_object.hpp"

namespace builtin_scene
{
  class MeshBase : public Transform
  {
  public:
    MeshBase();
    ~MeshBase();
  };
}
