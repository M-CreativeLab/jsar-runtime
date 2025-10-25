#pragma once

#include <iostream>
#include <concepts>
#include <vector>
#include <string>
#include <optional>
#include <rapidjson/document.h>
#include "./holocron_webgl.autogen.hpp"

namespace crates::webgl
{
  /**
   * GLSLPatcher is used to patch a GLSL source string.
   */
  class GLSLSourcePatcher2
  {
  public:
    /**
     * Get the patched GLSL source string.
     *
     * @param source The GLSL source string to patch.
     * @returns The patched GLSL source string.
     */
    static inline std::string GetPatchedSource(const std::string &source)
    {
      auto patched = holocron::webgl::patchGLSLSourceFromStr(source.c_str());
      return std::string(patched);
    }
  };

  /**
   * Represents a GLSL vertex attribute.
   */
  struct GLSLAttribute
  {
    std::string name;      ///< Attribute name (e.g., "position", "normal")
    std::string type_name; ///< Attribute type (e.g., "vec3", "vec4")
    int32_t location;      ///< Attribute location (0-based index)
  };

  /**
   * Represents a GLSL uniform variable.
   */
  struct GLSLUniform
  {
    std::string name;      ///< Uniform name (e.g., "modelViewMatrix", "lightColor")
    std::string type_name; ///< Uniform type (e.g., "mat4", "vec3", "sampler2D")
  };

  /**
   * GLSLShaderAnalyzer parses GLSL shader source code to extract variable information.
   * 
   * This class provides a convenient C++ interface to parse GLSL shaders and
   * extract all attribute and uniform declarations, including their names, types, and locations.
   * 
   * Features:
   * - Supports both GLSL 100 ES (attribute) and GLSL 300 ES (in) qualifiers
   * - Handles explicit layout(location = N) qualifiers
   * - Auto-assigns locations based on declaration order when not explicitly specified
   * - Filters out inactive (unreferenced) attributes and uniforms
   * - Compatible with WebGL 1.0 and WebGL 2.0 shaders
   * 
   * Example usage:
   * @code
   *   std::string vertexShader = R"(
   *     #version 300 es
   *     in vec3 position;
   *     in vec3 normal;
   *     uniform mat4 mvpMatrix;
   *     void main() {
   *       gl_Position = mvpMatrix * vec4(position, 1.0);
   *     }
   *   )";
   *   
   *   auto attributes = GLSLShaderAnalyzer::ParseAttributes(vertexShader);
   *   for (const auto& attr : attributes) {
   *     std::cout << attr.name << " at location " << attr.location << std::endl;
   *   }
   *   
   *   auto uniforms = GLSLShaderAnalyzer::ParseUniforms(vertexShader);
   *   for (const auto& uniform : uniforms) {
   *     std::cout << uniform.name << " (" << uniform.type_name << ")" << std::endl;
   *   }
   * @endcode
   */
  class GLSLShaderAnalyzer
  {
  public:
    /**
     * Parse GLSL vertex shader source and extract all attribute declarations.
     * 
     * @param source The GLSL vertex shader source code as a string.
     * @returns A vector of GLSLAttribute structs containing attribute metadata.
     */
    static inline std::vector<GLSLAttribute> ParseAttributes(const std::string &source)
    {
      auto json_str = holocron::webgl::parseGLSLAttributes(source.c_str());
      std::vector<GLSLAttribute> attributes;

      rapidjson::Document doc;
      doc.Parse(std::string(json_str).c_str());

      if (!doc.IsArray())
      {
        return attributes;
      }

      attributes.reserve(doc.Size());
      for (rapidjson::SizeType i = 0; i < doc.Size(); i++)
      {
        const auto &attr = doc[i];
        if (attr.IsObject() && attr.HasMember("name") && attr.HasMember("type_name") && attr.HasMember("location"))
        {
          attributes.push_back(GLSLAttribute{
            attr["name"].GetString(),
            attr["type_name"].GetString(),
            attr["location"].GetInt()});
        }
      }

      return attributes;
    }

    /**
     * Parse GLSL shader source and extract all uniform declarations.
     * 
     * @param source The GLSL shader source code as a string.
     * @returns A vector of GLSLUniform structs containing uniform metadata.
     */
    static inline std::vector<GLSLUniform> ParseUniforms(const std::string &source)
    {
      auto json_str = holocron::webgl::parseGLSLUniforms(source.c_str());
      std::vector<GLSLUniform> uniforms;

      rapidjson::Document doc;
      doc.Parse(std::string(json_str).c_str());

      if (!doc.IsArray())
      {
        return uniforms;
      }

      uniforms.reserve(doc.Size());
      for (rapidjson::SizeType i = 0; i < doc.Size(); i++)
      {
        const auto &uniform = doc[i];
        if (uniform.IsObject() && uniform.HasMember("name") && uniform.HasMember("type_name"))
        {
          uniforms.push_back(GLSLUniform{
            uniform["name"].GetString(),
            uniform["type_name"].GetString()});
        }
      }

      return uniforms;
    }

    /**
     * Get the location of a specific attribute by name.
     * 
     * @param source The GLSL vertex shader source code as a string.
     * @param name The name of the attribute to look up.
     * @returns An optional containing the attribute location if found, or std::nullopt if not found.
     */
    static inline std::optional<int32_t> GetAttribLocation(const std::string &source, const std::string &name)
    {
      auto attributes = ParseAttributes(source);
      for (const auto &attr : attributes)
      {
        if (attr.name == name)
        {
          return attr.location;
        }
      }
      return std::nullopt;
    }
  };
}
