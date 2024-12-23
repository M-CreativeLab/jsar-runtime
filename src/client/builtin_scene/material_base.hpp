#pragma once

#include "./asset.hpp"
#include "./shader_base.hpp"

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
    static std::shared_ptr<MaterialType> Make(Args &&...args)
    {
      return std::make_shared<MaterialType>(std::forward<Args>(args)...);
    }

  public:
    Material() = default;
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
  };
}
