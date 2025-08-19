#pragma once

#include <memory>
#include <cassert>
#include <concepts>
#include <client/graphics/webgl_context.hpp>

#include "./ecs.hpp"
#include "./material_base.hpp"
#include "./renderer/render_pass.hpp"
#include "./renderer/render_target.hpp"

namespace builtin_scene
{
  class MeshMaterial3d : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    MeshMaterial3d(std::shared_ptr<Material> material)
        : ecs::Component()
        , material_(material)
    {
    }

  public:
    /**
     * @returns The material handle.
     */
    template <typename MaterialType = Material>
      requires std::is_base_of<Material, MaterialType>::value
    inline std::shared_ptr<MaterialType> material() const
    {
      if (material_ == nullptr)
        return nullptr;

      // If the material type is the same as the base type, return the handle as is.
      if constexpr (std::is_same<MaterialType, Material>::value)
        return material_;

      // Downcast the material to the specified type.
      return std::dynamic_pointer_cast<MaterialType>(material_);
    }
    /**
     * @returns The mesh handle.
     */
    inline std::shared_ptr<Mesh3d> mesh() const
    {
      return mesh_;
    }
    /**
     * @returns The program of the material.
     */
    inline std::shared_ptr<client_graphics::WebGLProgram> program() const
    {
      return program_;
    }

    inline bool isOpaque() const
    {
      return material_->isOpaque();
    }
    inline bool isTransparent() const
    {
      return !isOpaque();
    }

    /**
     * Check if the material matches the given render pass.
     *
     * @param pass The render pass to check against.
     * @returns Whether the material matches the render pass.
     */
    bool matchesPass(const RenderPass &pass) const;

    /**
     * Initialize the `MeshMaterial3d` instance with the given WebGL context and program.
     *
     * @param glContext The WebGL context to use.
     * @param program The WebGL program to use.
     */
    void initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                    std::shared_ptr<client_graphics::WebGLProgram> program,
                    std::shared_ptr<Mesh3d> mesh);
    /**
     * @returns Whether the material is initialized.
     */
    inline bool initialized() const
    {
      return initialized_;
    }
    /**
     * Get the shader source of this material.
     *
     * @param type The type of the shader, either vertex or fragment.
     * @returns The shader source.
     * @throws std::runtime_error If the shader type is not vertex or fragment.
     */
    inline std::string getShaderSource(client_graphics::WebGLShaderType type) const
    {
      const auto &defines = material_->getDefinesWithGlobals();
      if (type == client_graphics::WebGLShaderType::kVertex)
        return material_->vertexShader().shader(defines).source;
      else if (type == client_graphics::WebGLShaderType::kFragment)
        return material_->fragmentShader().shader(defines).source;
      else
        throw std::runtime_error("The shader type is not supported.");
    }
    /**
     * Custom drawing implementation for materials that need special rendering logic.
     * Return true if the material handled the drawing, false to use default drawing.
     * 
     * @param mesh The mesh to draw.
     * @param renderPass The render pass to use, which can be used to filter the objects or instances to render.
     * @param renderTarget The render target to draw the mesh with.
     */
    inline void drawMeshImpl(std::shared_ptr<Mesh3d> mesh,
                             RenderPass renderPass,
                             std::optional<XRRenderTarget> renderTarget)
    {
      assert(material_ != nullptr);
      material_->drawMeshImpl(program_, *mesh, renderPass, renderTarget);
    }
    /**
     * Called before drawing the mesh with the material.
     * 
     * @param mesh The mesh to draw.
     */
    inline void onBeforeDrawMesh(std::shared_ptr<Mesh3d> mesh)
    {
      assert(material_ != nullptr);
      material_->onBeforeDrawMesh(program_, mesh);
    }
    /**
     * Called after drawing the mesh with the material.
     * 
     * @param mesh The mesh to draw.
     */
    inline void onAfterDrawMesh(std::shared_ptr<Mesh3d> mesh)
    {
      assert(material_ != nullptr);
      material_->onAfterDrawMesh(program_, mesh);
    }

  private:
    std::shared_ptr<Material> material_ = nullptr;
    std::shared_ptr<Mesh3d> mesh_ = nullptr;
    std::shared_ptr<client_graphics::WebGLProgram> program_;
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    bool initialized_ = false;
  };
}
