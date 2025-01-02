#pragma once

#include <vector>
#include <string>
#include "./shaders_store.gen.hpp"

namespace builtin_scene
{
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
     * @return The preprocessed source code of the shader.
     */
    static std::string PreprocessSource(std::string source, const std::vector<std::string> &defines)
    {
      // Add WebGL 2.0 version: #version 300 es
      source = "#version 300 es\n" + source;

      // Add the defines
      for (const auto &define : defines)
        source = "#define " + define + "\n" + source;

      // Return the preprocessed source
      return source;
    }

  public:
    ShaderPreprocessor() = delete;
  };

  /**
   * The `ShaderSource` class represents a shader source code.
   */
  class ShaderSource
  {
  public:
    ShaderSource(std::string name, std::string source, const std::vector<std::string> &defines)
        : name(name), source(ShaderPreprocessor::PreprocessSource(source, defines))
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
    ShaderRef(std::string name) : name(name)
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
      return ShaderSource(name, shaders::SHADERS_STORE.at(name), defines);
    }

  public:
    /**
     * The relative path of the builtin shader, such as "materials/color.frag".
     */
    std::string name;
  };
}
