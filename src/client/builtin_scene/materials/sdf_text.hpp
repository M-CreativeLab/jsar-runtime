#pragma once

#include "../material_base.hpp"
#include <memory>
#include <glm/glm.hpp>

namespace builtin_scene::materials
{
  /**
   * Material for rendering SDF-based text
   */
  class SDFTextMaterial : public Material
  {
  public:
    SDFTextMaterial();
    explicit SDFTextMaterial(uint32_t textureId);
    ~SDFTextMaterial() = default;

    // Material properties
    void setSDFTexture(uint32_t textureId)
    {
      sdfTextureId_ = textureId;
    }
    void setSDFSpread(float spread)
    {
      sdfSpread_ = spread;
    }
    void setSDFThickness(float thickness)
    {
      sdfThickness_ = thickness;
    }
    void setSDFSoftness(float softness)
    {
      sdfSoftness_ = softness;
    }
    void setTextColor(float r, float g, float b, float a = 1.0f);
    void setOpacity(float opacity)
    {
      opacity_ = opacity;
    }

    // Getters
    uint32_t getSDFTexture() const
    {
      return sdfTextureId_;
    }
    float getSDFSpread() const
    {
      return sdfSpread_;
    }
    float getSDFThickness() const
    {
      return sdfThickness_;
    }
    float getSDFSoftness() const
    {
      return sdfSoftness_;
    }
    const glm::vec4 &getTextColor() const
    {
      return textColor_;
    }
    float getOpacity() const
    {
      return opacity_;
    }

    // Material interface
    const std::string name() const override
    {
      return "SDFTextMaterial";
    }
    const std::vector<std::string> defines() const override;
    ShaderRef fragmentShader() override;
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program) override;

    // Factory methods
    static std::shared_ptr<SDFTextMaterial> create();
    static std::shared_ptr<SDFTextMaterial> create(uint32_t textureId);

  private:
    uint32_t sdfTextureId_;
    float sdfSpread_;
    float sdfThickness_;
    float sdfSoftness_;
    glm::vec4 textColor_;
    float opacity_;

    void initializeDefaults();
  };
}