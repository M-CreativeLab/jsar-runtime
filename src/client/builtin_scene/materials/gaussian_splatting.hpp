#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../material_base.hpp"

namespace builtin_scene::materials
{
  /**
   * Material for rendering 3D Gaussian Splatting models.
   */
  class GaussianSplattingMaterial : public Material
  {
  public:
    struct GaussianSplat
    {
      glm::vec3 position;
      glm::vec3 color;
      float opacity;
      glm::vec3 scale;
      glm::vec4 rotation; // quaternion
    };

  public:
    GaussianSplattingMaterial() = default;

  public:
    const std::string name() const override
    {
      return "GaussianSplattingMaterial";
    }
    
    const std::vector<std::string> defines() const override
    {
      return mixDefines(Material::defines(), {});
    }
    
    ShaderRef vertexShader() override
    {
      return ShaderRef(ShaderType::kVertex, "shaders/gaussian_splatting.vert");
    }
    
    ShaderRef fragmentShader() override
    {
      return ShaderRef(ShaderType::kFragment, "shaders/gaussian_splatting.frag");
    }
    
    bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program) override;
    
    void onBeforeDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                          std::shared_ptr<Mesh3d> mesh) override;
    
    void onAfterDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                         std::shared_ptr<Mesh3d> mesh) override;

  public:
    /**
     * Update the gaussian splats data for rendering.
     */
    void updateSplats(const std::vector<GaussianSplat> &splats);
    
    /**
     * Get the current number of splats.
     */
    size_t getSplatCount() const { return splats_.size(); }

  private:
    std::vector<GaussianSplat> splats_;
    
    // WebGL buffer objects
    client_graphics::WebGLBuffer positionBuffer_;
    client_graphics::WebGLBuffer colorBuffer_;
    client_graphics::WebGLBuffer opacityBuffer_;
    client_graphics::WebGLBuffer scaleBuffer_;
    client_graphics::WebGLBuffer rotationBuffer_;
    
    bool buffersInitialized_ = false;
  };
}