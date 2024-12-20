#pragma once

#include <glm/glm.hpp>
#include "../material_base.hpp"

namespace builtin_scene::materials
{
  class NormalMaterial : Material
  {
  public:
    NormalMaterial() = default;

  public:
    ShaderRef fragmentShader() override
    {
      return ShaderRef("materials/normal.frag");
    }
  };
}
