#pragma once

#include <glm/glm.hpp>
#include "../material_base.hpp"

namespace builtin_scene::materials
{
  class ColorMaterial : public Material
  {
  public:
    ColorMaterial() = default;
    ColorMaterial(float red, float green, float blue)
        : color(red, green, blue) {}

  public:
    ShaderRef fragmentShader() override
    {
      return ShaderRef("materials/color.frag");
    }

  public:
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
  };
}
