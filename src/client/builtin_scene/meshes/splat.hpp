#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../mesh_base.hpp"
#include "./builder.hpp"

namespace builtin_scene::meshes
{
  /**
   * Base mesh class for Gaussian splat rendering.
   * Provides a simple quad geometry for instanced splat rendering.
   */
  class Splat : public Mesh,
                public MeshBuilder,
                public Primitive3d
  {
  public:
    Splat();
    virtual ~Splat() = default;

  public:
    void build() override;
    float area() override;
    float volume() override;

  private:
    void createQuadGeometry();
  };
}