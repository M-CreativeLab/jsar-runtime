#pragma once

#include "../mesh_base.hpp"

namespace builtin_scene
{
  namespace meshes
  {
    class Plane : MeshBase
    {
      using MeshBase::MeshBase;

    public:
      Plane(float width, float height);
    };
  }
}
