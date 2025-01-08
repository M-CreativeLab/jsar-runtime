#pragma once

#include <memory>
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
    MeshMaterial3d(std::shared_ptr<Material> handle)
        : ecs::Component(),
          handle_(handle)
    {
    }

  public:
    inline std::shared_ptr<client_graphics::WebGLProgram> program() const
    {
      return program_;
    }
    inline std::shared_ptr<Material> material() const { return handle_; }
    /**
     * Initialize the `MeshMaterial3d` instance with the given WebGL context and program.
     *
     * @param glContext The WebGL context to use.
     * @param program The WebGL program to use.
     */
    inline void initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                           std::shared_ptr<client_graphics::WebGLProgram> program)
    {
      if (program == nullptr)
        throw std::runtime_error("The program is not initialized.");

      if (handle_->initialize(glContext, program))
      {
        program_ = program;
        initialized_ = true;
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
      const auto& defines = handle_->defines();
      if (type == client_graphics::WebGLShaderType::kVertex)
        return handle_->vertexShader().shader(defines).source;
      else if (type == client_graphics::WebGLShaderType::kFragment)
        return handle_->fragmentShader().shader(defines).source;
      else
        throw std::runtime_error("The shader type is not supported.");
    }

  private:
    std::shared_ptr<Material> handle_ = nullptr;
    std::shared_ptr<client_graphics::WebGLProgram> program_;
    bool initialized_ = false;
  };
}
