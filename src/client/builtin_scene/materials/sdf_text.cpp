#include "./sdf_text.hpp"
#include <iostream>

namespace builtin_scene::materials
{
  SDFTextMaterial::SDFTextMaterial()
  {
    initializeDefaults();
  }

  SDFTextMaterial::SDFTextMaterial(uint32_t textureId)
      : sdfTextureId_(textureId)
  {
    initializeDefaults();
  }

  void SDFTextMaterial::setTextColor(float r, float g, float b, float a)
  {
    textColor_ = glm::vec4(r, g, b, a);
  }

  const std::vector<std::string> SDFTextMaterial::defines() const
  {
    return {"USE_SDF_TEXTURE"};
  }

  ShaderRef SDFTextMaterial::fragmentShader()
  {
    return ShaderRef(client_graphics::WebGLShaderType::kFragment, "shaders/sdf_text.frag");
  }

  bool SDFTextMaterial::initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                                   std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    if (!Material::initialize(glContext, program))
      return false;

    // Get uniform locations and set values
    auto sdfTextureLocation = glContext->getUniformLocation(program, "sdfTexture");
    if (!sdfTextureLocation.has_value())
    {
      std::cerr << name() << ": The sdfTexture uniform location is not found." << std::endl;
      return false;
    }

    auto sdfSpreadLocation = glContext->getUniformLocation(program, "sdfSpread");
    if (!sdfSpreadLocation.has_value())
    {
      std::cerr << name() << ": The sdfSpread uniform location is not found." << std::endl;
      return false;
    }

    auto sdfThicknessLocation = glContext->getUniformLocation(program, "sdfThickness");
    if (!sdfThicknessLocation.has_value())
    {
      std::cerr << name() << ": The sdfThickness uniform location is not found." << std::endl;
      return false;
    }

    auto sdfSoftnessLocation = glContext->getUniformLocation(program, "sdfSoftness");
    if (!sdfSoftnessLocation.has_value())
    {
      std::cerr << name() << ": The sdfSoftness uniform location is not found." << std::endl;
      return false;
    }

    auto textColorLocation = glContext->getUniformLocation(program, "textColor");
    if (!textColorLocation.has_value())
    {
      std::cerr << name() << ": The textColor uniform location is not found." << std::endl;
      return false;
    }

    auto opacityLocation = glContext->getUniformLocation(program, "opacity");
    if (!opacityLocation.has_value())
    {
      std::cerr << name() << ": The opacity uniform location is not found." << std::endl;
      return false;
    }

    // Set uniform values
    glContext->uniform1i(sdfTextureLocation.value(), 0); // Texture unit 0
    glContext->uniform1f(sdfSpreadLocation.value(), sdfSpread_);
    glContext->uniform1f(sdfThicknessLocation.value(), sdfThickness_);
    glContext->uniform1f(sdfSoftnessLocation.value(), sdfSoftness_);
    glContext->uniform4f(textColorLocation.value(), textColor_.r, textColor_.g, textColor_.b, textColor_.a);
    glContext->uniform1f(opacityLocation.value(), opacity_);

    return true;
  }

  std::shared_ptr<SDFTextMaterial> SDFTextMaterial::create()
  {
    return std::make_shared<SDFTextMaterial>();
  }

  std::shared_ptr<SDFTextMaterial> SDFTextMaterial::create(uint32_t textureId)
  {
    return std::make_shared<SDFTextMaterial>(textureId);
  }

  void SDFTextMaterial::initializeDefaults()
  {
    sdfTextureId_ = 0;
    sdfSpread_ = 8.0f;                              // Default spread (should match SDF generation params)
    sdfThickness_ = 0.5f;                           // Normal thickness
    sdfSoftness_ = 1.0f;                            // Slight edge softness
    textColor_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White text
    opacity_ = 1.0f;                                // Fully opaque
    isOpaque_ = false;                              // Text materials are typically transparent
  }
}