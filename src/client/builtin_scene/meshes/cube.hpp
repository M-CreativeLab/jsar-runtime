#pragma once

#include "../mesh_base.hpp"

namespace builtin_scene
{
  namespace meshes
  {
    class Cube : MeshBase
    {
      using MeshBase::MeshBase;

    public:
      Cube(float width, float height, float depth);
    };
  }
}
