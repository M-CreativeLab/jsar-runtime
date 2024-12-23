#pragma once

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
     * @return The preprocessed source code of the shader.
     */
    static std::string PreprocessSource(std::string source)
    {
      // Add WebGL 2.0 version: #version 300 es
      source = "#version 300 es\n" + source;
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
    ShaderSource(std::string name, std::string source)
        : name(name), source(ShaderPreprocessor::PreprocessSource(source))
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
     * @returns The `ShaderSource` object of the shader.
     */
    ShaderSource shader() const
    {
      return ShaderSource(name, shaders::SHADERS_STORE.at(name));
    }

  public:
    /**
     * The relative path of the builtin shader, such as "materials/color.frag".
     */
    std::string name;
  };
}
