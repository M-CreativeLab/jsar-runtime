#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../material_base.hpp"

namespace builtin_scene::materials
{
  class ColorMaterial : public Material
  {
    using ColorMaterialReference = std::shared_ptr<ColorMaterial>;

  public:
    /**
     * @returns The white color material.
     */
    static inline ColorMaterialReference White() { return std::make_shared<ColorMaterial>(1.0f, 1.0f, 1.0f); }
    /**
     * @returns The black color material.
     */
    static inline ColorMaterialReference Black() { return std::make_shared<ColorMaterial>(0.0f, 0.0f, 0.0f); }
    /**
     * @returns The red color material.
     */
    static inline ColorMaterialReference Red() { return std::make_shared<ColorMaterial>(1.0f, 0.0f, 0.0f); }
    /**
     * @returns The green color material.
     */
    static inline ColorMaterialReference Green() { return std::make_shared<ColorMaterial>(0.0f, 1.0f, 0.0f); }
    /**
     * @returns The blue color material.
     */
    static inline ColorMaterialReference Blue() { return std::make_shared<ColorMaterial>(0.0f, 0.0f, 1.0f); }

  public:
    /**
     * Create a new instance of the color material with no surface color (alpha is 0.0).
     */
    ColorMaterial() = default;
    /**
     * Create a new instance of the color material with the given surface color.
     *
     * @param red The red component of the surface color.
     * @param green The green component of the surface color.
     * @param blue The blue component of the surface color.
     * @param alpha The alpha component of the surface color.
     */
    ColorMaterial(float red, float green, float blue, float alpha = 1.0f)
        : surfaceColor(red, green, blue, alpha) {}

  public:
    const std::string name() const override { return "ColorMaterial"; }
    ShaderRef fragmentShader() override
    {
      return ShaderRef(ShaderType::kFragment, "materials/color.frag");
    }
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program) override
    {
      if (TR_UNLIKELY(!Material::initialize(glContext, program)))
        return false;

      auto surfaceColorLoc = glContext->getUniformLocation(program, "surfaceColor");
      if (TR_UNLIKELY(!surfaceColorLoc.has_value()))
      {
        std::cerr << name() << ": The surfaceColor uniform location is not found." << std::endl;
        return false;
      }

      glContext->uniform4f(surfaceColorLoc.value(),
                           surfaceColor.r, surfaceColor.g, surfaceColor.b, surfaceColor.a);
      return true;
    }

  public:
    glm::vec4 surfaceColor = {1.0f, 1.0f, 1.0f, 0.0f};
  };
}
