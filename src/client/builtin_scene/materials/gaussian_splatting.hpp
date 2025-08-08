#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../material_base.hpp"

namespace builtin_scene::materials
{
  /**
   * Material for rendering 3D Gaussian Splatting models using instanced rendering.
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
     * Update the gaussian splats data for instanced rendering.
     */
    void updateSplats(const std::vector<GaussianSplat> &splats);

    /**
     * Get the current number of splats.
     */
    size_t getSplatCount() const
    {
      return splats_.size();
    }

    /**
     * Draw the splats using instanced rendering.
     */
    void drawInstanced(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                       std::shared_ptr<client_graphics::WebGLProgram> program);

  private:
    std::vector<GaussianSplat> splats_;

    // WebGL buffer objects for instanced data
    std::shared_ptr<client_graphics::WebGLBuffer> splatPositionBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatColorBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatOpacityBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatScaleBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatRotationBuffer_;

    // Base quad geometry buffer
    std::shared_ptr<client_graphics::WebGLBuffer> quadVertexBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> quadIndexBuffer_;

    bool buffersInitialized_ = false;
    bool quadGeometryCreated_ = false;

    void createQuadGeometry(std::shared_ptr<client_graphics::WebGL2Context> glContext);
  };
}