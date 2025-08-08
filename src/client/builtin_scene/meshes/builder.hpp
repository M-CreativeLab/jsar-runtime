#pragma once

#include <memory>

namespace builtin_scene::meshes
{
  class MeshBuilder
  {
  public:
    MeshBuilder() = default;
    virtual ~MeshBuilder() = default;

  public:
    /**
     * The implemetor should build the mesh's vertices and indices in this method.
     */
    virtual void build() = 0;
  };
}
