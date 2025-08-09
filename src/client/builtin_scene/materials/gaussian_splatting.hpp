#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../material_base.hpp"
#include "../gaussian_splats_mesh.hpp"

namespace builtin_scene::materials
{
  /**
   * Material for rendering 3D Gaussian Splatting models using instanced rendering.
   * Works with GaussianSplatsMesh to render all splats globally.
   */
  class GaussianSplattingMaterial : public Material
  {
  public:
    GaussianSplattingMaterial()
        : Material(false)
    {
    } // Make transparent

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

    bool drawMeshImpl(std::shared_ptr<client_graphics::WebGLProgram> program,
                      std::shared_ptr<Mesh3d> mesh) override;

    void onAfterDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                         std::shared_ptr<Mesh3d> mesh) override;

  public:
    /**
     * Update the gaussian splats instance data for instanced rendering.
     */
    void updateSplatInstances(const std::vector<SplatInstanceData> &instances);

    /**
     * Get the current number of splat instances.
     */
    size_t getSplatInstanceCount() const
    {
      return splatInstances_.size();
    }

  private:
    /**
     * Setup instanced vertex attributes for splat rendering.
     */
    void setupInstancedAttributes(std::shared_ptr<client_graphics::WebGLProgram> program,
                                  std::shared_ptr<client_graphics::WebGL2Context> glContext);

  private:
    std::vector<SplatInstanceData> splatInstances_;

    // WebGL buffer objects for instanced data
    std::shared_ptr<client_graphics::WebGLBuffer> splatPositionBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatColorBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatOpacityBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatScaleBuffer_;
    std::shared_ptr<client_graphics::WebGLBuffer> splatRotationBuffer_;

    bool buffersInitialized_ = false;
  };
}