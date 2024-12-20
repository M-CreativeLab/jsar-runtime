#pragma once

#include "./asset.hpp"
#include "./shader_base.hpp"

namespace builtin_scene
{
  class Material
  {
  public:
    Material() = default;

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
