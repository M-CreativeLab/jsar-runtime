#pragma once

#include <vector>
#include <string>
#include <client/graphics/webgl_shader.hpp>
#include "./shaders_store.gen.hpp"

namespace builtin_scene
{
  using ShaderType = client_graphics::WebGLShaderType;

  /**
   * The preprocessor for shaders, such as adding #version, #extension, etc.
   */
  class ShaderPreprocessor
  {
  public:
    /**
     * Preprocess the source code of a shader.
     *
     * @param source The source code of the shader.
     * @param defines The list of defines to add to the shader.
     * @param shaderType The type of the shader, such as vertex or fragment shader.
     * @return The preprocessed source code of the shader.
     */
    static std::string PreprocessSource(std::string source, const std::vector<std::string> &defines,
                                        client_graphics::WebGLShaderType shaderType);

    /**
     * Concatenate the source code with the given lines.
     *
     * @param source The source code to concatenate.
     * @param lines The list of lines to concatenate.
     * @return The concatenated source code.
     */
    static std::string ConcatSource(const std::string &source, const std::vector<std::string> &lines);

  public:
    ShaderPreprocessor() = delete;
  };

  /**
   * The `ShaderSource` class represents a shader source code.
   */
  class ShaderSource
  {
  public:
    ShaderSource(std::string name, std::string source, const std::vector<std::string> &defines,
                 client_graphics::WebGLShaderType shaderType)
        : name(name), source(ShaderPreprocessor::PreprocessSource(source, defines, shaderType))
    {
    }

  public:
    std::string name;
    std::string source;
  };

  /**
   * A reference to a `ShaderSource` object.
   */
  class ShaderRef
  {
  public:
    /**
     * Construct a `ShaderRef` object with the given name.
     *
     * @param name The relative path of the builtin shader, such as "materials/color.frag".
     */
    ShaderRef(client_graphics::WebGLShaderType type, std::string name) : type(type), name(name)
    {
    }

  public:
    /**
     * Get the `ShaderSource` object of the shader.
     *
     * @param defines The list of defines to add to the shader.
     * @returns The `ShaderSource` object.
     */
    ShaderSource shader(const std::vector<std::string> &defines = {}) const
    {
      if (shaders::SHADERS_STORE.find(name) == shaders::SHADERS_STORE.end())
        throw std::runtime_error("The shader is not found: " + name);
      return ShaderSource(name, shaders::SHADERS_STORE.at(name), defines, type);
    }

  public:
    /**
     * The shader type, such as vertex or fragment shader.
     */
    client_graphics::WebGLShaderType type;
    /**
     * The relative path of the builtin shader, such as "materials/color.frag".
     */
    std::string name;
  };
}
