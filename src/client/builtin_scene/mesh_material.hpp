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
    inline std::shared_ptr<client_graphics::WebGLProgram> program()
    {
      return program_;
    }
    inline void initialize(std::shared_ptr<client_graphics::WebGLProgram> program)
    {
      if (program == nullptr)
        throw std::runtime_error("The program is not initialized.");

      program_ = program;
      initialized_ = true;
    }
    inline bool initialized() { return initialized_; }
    inline ShaderRef vertexShader() const
    {
      return handle_->vertexShader();
    }
    inline ShaderRef fragmentShader() const
    {
      return handle_->fragmentShader();
    }

  private:
    std::shared_ptr<Material> handle_ = nullptr;
    std::shared_ptr<client_graphics::WebGLProgram> program_;
    bool initialized_ = false;
  };
}
