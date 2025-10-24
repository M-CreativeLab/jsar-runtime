#pragma once

#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

#include "../meshes.hpp"
#include "../material_base.hpp"
#include "../texture_altas.hpp"
#include "../web_content.hpp"
#include "../css_border_data_texture.hpp"
#include "./color.hpp"

namespace endor
{
  namespace builtin_scene::materials
  {
    class WebContentInstancedMaterial final : public Material
    {
    public:
      enum class TextureUpdateStatus : uint8_t
      {
        kSkipped,
        kSuccess,
        kFailed
      };

    public:
      WebContentInstancedMaterial();

    public:
      const std::string name() const override
      {
        return "WebContentInstancedMaterial";
      }
      const std::vector<std::string> defines() const override
      {
        return mixDefines(Material::defines(),
                          {
                            "USE_UVS",
                            "USE_INSTANCE_TRANSFORMS",
                            "USE_INSTANCE_COLORS",
                            "USE_INSTANCE_TEXTURE",
                            "USE_INSTANCE_SDF"
                            // End
                          });
      }
      ShaderRef fragmentShader() override
      {
        return ShaderRef(ShaderType::kFragment, "materials/web_content.frag");
      }
      bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                      std::shared_ptr<client_graphics::WebGLProgram> program) override;
      void drawMeshImpl(std::shared_ptr<client_graphics::WebGLProgram> program,
                        const Mesh3d &mesh,
                        RenderPass renderPass,
                        std::optional<XRRenderTarget> renderTarget) override;
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
     * @returns The status of the texture update.
     */
      TextureUpdateStatus updateTexture(WebContent &content);

      /**
     * Enable or disable SDF-based anti-aliasing globally.
     *
     * @param enabled Whether to enable SDF rendering.
     */
      void setSdfEnabled(bool enabled);

      /**
     * Get the border data texture for direct updates.
     *
     * @return Pointer to the CSSBorderDataTexture instance.
     */
      CSSBorderDataTexture *getBorderDataTexture() const;

    public:
      float width() const
      {
        return width_;
      }
      float height() const
      {
        return height_;
      }

    private:
      inline client_graphics::WebGLUniformLocation uniform(const std::string &name) const
      {
        auto it = uniforms_.find(name);
        if (it == uniforms_.end())
          throw std::runtime_error("The uniform " + name + " is not found.");
        return it->second;
      }

      inline bool hasUniform(const std::string &name) const
      {
        return uniforms_.find(name) != uniforms_.end();
      }

    private:
      float width_;
      float height_;
      glm::vec2 textureOffset_ = glm::vec2(0.0f, 0.0f);
      glm::vec2 textureScale_ = glm::vec2(1.0f, 1.0f);
      std::unordered_map<std::string, client_graphics::WebGLUniformLocation> uniforms_;
      std::unique_ptr<TextureAtlas> textureAtlas_;

      // SDF rendering parameters
      bool sdfEnabled_ = true;

      // Border data texture manager
      std::unique_ptr<CSSBorderDataTexture> borderDataTexture_;
    };
  }
} // namespace endor
