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
    static std::shared_ptr<GaussianSplattingMaterial> Default()
    {
      return std::make_shared<GaussianSplattingMaterial>();
    }

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

    void drawMeshImpl(std::shared_ptr<client_graphics::WebGLProgram> program,
                      const Mesh3d &mesh,
                      RenderPass renderPass,
                      std::optional<XRRenderTarget> renderTarget) override;
    void onBeforeDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                          std::shared_ptr<Mesh3d> mesh) override;

  public:
    /**
     * Get the current number of splat instances.
     */
    size_t getSplatInstanceCount() const
    {
      return splatInstanceCount_;
    }

  private:
    size_t splatInstanceCount_ = 0;
  };
}