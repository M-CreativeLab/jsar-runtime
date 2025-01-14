#pragma once

#include <memory>

#include "../meshes.hpp"
#include "../material_base.hpp"
#include "../web_content.hpp"
#include "./color.hpp"

namespace builtin_scene::materials
{
  class WebContentMaterial final : public ColorMaterial
  {
  public:
    WebContentMaterial() = default;
    WebContentMaterial(float width, float height)
        : ColorMaterial(), width_(width), height_(height)
    {
    }

  public:
    const std::string name() const override
    {
      return "WebContentMaterial";
    }
    const std::vector<std::string> defines() const override
    {
      return {"USE_UVS", "USE_TEXTURE"};
    }
    ShaderRef fragmentShader() override
    {
      return ShaderRef("materials/color.frag");
    }
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program) override;
    void onBeforeDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                          std::shared_ptr<Mesh3d> mesh) override;
    void onAfterDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                         std::shared_ptr<Mesh3d> mesh) override;

  public:
    /**
     * Update the texture from the given WebContent.
     *
     * @param content The WebContent to update the material with.
     */
    void updateTexture(WebContent &content);

  public:
    float width() const { return width_; }
    float height() const { return height_; }

  private:
    float width_;
    float height_;
    std::shared_ptr<client_graphics::WebGLTexture> texture_;
  };
}
