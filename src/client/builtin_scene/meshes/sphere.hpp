#pragma once

#include "../mesh_base.hpp"

namespace builtin_scene
{
  namespace meshes
  {
    class Sphere : MeshBase
    {
      using MeshBase::MeshBase;

    public:
      Sphere(float radius, int segments);
    };
  }
}
