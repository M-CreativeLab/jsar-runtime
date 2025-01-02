#pragma once

#include "./asset.hpp"
#include "./shader_base.hpp"
#include <client/graphics/webgl_context.hpp>
#include <client/graphics/webgl_program.hpp>

namespace builtin_scene
{
  class Material
  {
  public:
    /**
     * Create a new instance of the material.
     *
     * @tparam MaterialType The type of the material.
     * @tparam Args The types of the arguments for the constructor of the material.
     * @param args The arguments for the constructor of the material.
     */
    template <typename MaterialType, typename... Args>
    static inline std::shared_ptr<MaterialType> Make(Args &&...args)
    {
      return std::make_shared<MaterialType>(std::forward<Args>(args)...);
    }

  public:
    Material()
    {
    }
    virtual ~Material() = default;

  public:
    /**
     * @returns The vertex shader for the material.
     */
    virtual ShaderRef vertexShader()
    {
      return ShaderRef("shaders/mesh.vert");
    }
    /**
     * @returns The fragment shader for the material.
     */
    virtual ShaderRef fragmentShader()
    {
      return ShaderRef("materials/default.frag");
    }
    /**
     * Initialize the material with the given program.
     *
     * @param glContext The WebGL context to initialize the material with.
     * @param program The WebGL program to initialize the material with.
     * @returns Whether the material is initialized successfully.
     */
    virtual bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                            std::shared_ptr<client_graphics::WebGLProgram> program)
    {
      return true;
    }
  };
}
