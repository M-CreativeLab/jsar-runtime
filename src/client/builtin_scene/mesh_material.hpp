#pragma once

#include <memory>
#include <concepts>
#include <client/graphics/webgl_context.hpp>

#include "./ecs.hpp"
#include "./material_base.hpp"

namespace builtin_scene
{
  class MeshMaterial3d : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    MeshMaterial3d(std::shared_ptr<Material> material)
        : ecs::Component(),
          material_(material)
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
    inline std::shared_ptr<Mesh3d> mesh() const { return mesh_; }
    /**
     * @returns The program of the material.
     */
    inline std::shared_ptr<client_graphics::WebGLProgram> program() const { return program_; }
    /**
     * Initialize the `MeshMaterial3d` instance with the given WebGL context and program.
     *
     * @param glContext The WebGL context to use.
     * @param program The WebGL program to use.
     */
    inline void initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                           std::shared_ptr<client_graphics::WebGLProgram> program,
                           std::shared_ptr<Mesh3d> mesh)
    {
      if (program == nullptr)
        throw std::runtime_error("The program is not initialized.");

      if (material_->initialize(glContext, program))
      {
        program_ = program;
        glContext_ = glContext;
        mesh_ = mesh;
        initialized_ = true;
      }
      else
      {
        throw std::runtime_error("Failed to initialize the material: " + material_->name());
      }
    }
    /**
     * @returns Whether the material is initialized.
     */
    inline bool initialized() const { return initialized_; }
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
