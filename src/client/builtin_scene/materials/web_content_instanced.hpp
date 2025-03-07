#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../meshes.hpp"
#include "../material_base.hpp"
#include "../texture_altas.hpp"
#include "../web_content.hpp"
#include "./color.hpp"

namespace builtin_scene::materials
{
  class WebContentInstancedMaterial final : public ColorMaterial
  {
  public:
    WebContentInstancedMaterial();

  public:
    const std::string name() const override
    {
      return "WebContentInstancedMaterial";
    }
    const std::vector<std::string> defines() const override
    {
      return mixDefines(ColorMaterial::defines(),
                        {
                            "USE_UVS",
                            //  "USE_TEXTURE",
                            "USE_INSTANCE_TRANSFORMS",
                            "USE_INSTANCE_COLORS",
                            "USE_INSTANCE_TEXTURE"
                            // End
                        });
    }
    ShaderRef fragmentShader() override
    {
      return ShaderRef(ShaderType::kFragment, "materials/color.frag");
    }
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program) override;
    void onBeforeDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                          std::shared_ptr<Mesh3d> mesh) override;
    void onAfterDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                         std::shared_ptr<Mesh3d> mesh) override;

  public:
    /**
     * Flip the texture by the Y-axis.
     */
    void flipTextureByY(bool flip);
    /**
     * Update the texture from the given WebContent.
     *
     * @param content The WebContent to update the material with.
     * @returns Whether the texture is updated successfully.
     */
    bool updateTexture(WebContent &content);

  public:
    float width() const { return width_; }
    float height() const { return height_; }
    float globalAspectRatio() const { return globalAspectRatio_; }
    void setGlobalAspectRatio(float aspectRatio) { globalAspectRatio_ = aspectRatio; }

  private:
    float width_;
    float height_;
    float globalAspectRatio_ = 1.0f;
    glm::vec2 textureOffset_ = glm::vec2(0.0f, 0.0f);
    glm::vec2 textureScale_ = glm::vec2(1.0f, 1.0f);
    std::unique_ptr<TextureAtlas> textureAtlas_;
  };
}
