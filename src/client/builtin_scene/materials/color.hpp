#pragma once

#include <glm/glm.hpp>
#include "../material_base.hpp"

namespace builtin_scene::materials
{
  class ColorMaterial final : public Material
  {
  public:
    /**
     * Create a new instance of the color material with the white surface color.
     */
    ColorMaterial() = default;
    /**
     * Create a new instance of the color material with the given surface color.
     * 
     * @param red The red component of the surface color.
     * @param green The green component of the surface color.
     * @param blue The blue component of the surface color.
     */
    ColorMaterial(float red, float green, float blue)
        : surfaceColor(red, green, blue) {}

  public:
    ShaderRef fragmentShader() override
    {
      return ShaderRef("materials/color.frag");
    }
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program) override
    {
      auto surfaceColorLoc = glContext->getUniformLocation(program, "surfaceColor");
      if (!surfaceColorLoc.has_value())
        return false;

      glContext->uniform4f(surfaceColorLoc.value(),
                           surfaceColor.r, surfaceColor.g, surfaceColor.b, 1.0f);
      return true;
    }

  public:
    glm::vec3 surfaceColor = {1.0f, 1.0f, 1.0f};
  };
}
