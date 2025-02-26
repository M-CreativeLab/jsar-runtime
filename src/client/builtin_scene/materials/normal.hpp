#pragma once

#include <glm/glm.hpp>
#include "../material_base.hpp"

namespace builtin_scene::materials
{
  class NormalMaterial : public Material
  {
  public:
    NormalMaterial() = default;

  public:
    const std::string name() const override { return "NormalMaterial"; }
    const std::vector<std::string> defines() const override
    {
      return {"USE_NORMALS"};
    }
    ShaderRef fragmentShader() override
    {
      return ShaderRef(ShaderType::kFragment, "materials/normal.frag");
    }
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program) override
    {
      auto normalMatrixLoc = glContext->getUniformLocation(program, "normalMatrix");
      if (!normalMatrixLoc.has_value())
        return false;

      glContext->uniformMatrix4fv(normalMatrixLoc.value(), false, glm::mat4(1.0f));
      return true;
    }
  };
}
