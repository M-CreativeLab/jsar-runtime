#pragma once

#include <iostream>
#include <concepts>
#include <vector>
#include <string>
#include <optional>
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
   * GLSLAttributeParser parses vertex shader source code to extract attribute information.
   * 
   * This class provides a convenient C++ interface to parse GLSL vertex shaders and
   * extract all attribute declarations, including their names, types, and assigned locations.
   * 
   * Features:
   * - Supports both GLSL 100 ES (attribute) and GLSL 300 ES (in) qualifiers
   * - Handles explicit layout(location = N) qualifiers
   * - Auto-assigns locations based on declaration order when not explicitly specified
   * - Compatible with WebGL 1.0 and WebGL 2.0 shaders
   * 
   * Example usage:
   * @code
   *   std::string vertexShader = R"(
   *     #version 300 es
   *     in vec3 position;
   *     in vec3 normal;
   *     void main() {
   *       gl_Position = vec4(position, 1.0);
   *     }
   *   )";
   *   
   *   auto attributes = GLSLAttributeParser::ParseAttributes(vertexShader);
   *   for (const auto& attr : attributes) {
   *     std::cout << attr.name << " at location " << attr.location << std::endl;
   *   }
   *   
   *   // Or get a specific attribute location
   *   auto location = GLSLAttributeParser::GetAttribLocation(vertexShader, "position");
   *   if (location.has_value()) {
   *     std::cout << "position is at location " << location.value() << std::endl;
   *   }
   * @endcode
   */
  class GLSLAttributeParser
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
      try
      {
        auto result = holocron::webgl::parseGLSLAttributes(source.c_str());
        std::vector<GLSLAttribute> attributes;
        attributes.reserve(result.attributes.size());

        for (const auto &attr : result.attributes)
        {
          attributes.push_back(GLSLAttribute{
            std::string(attr.name),
            std::string(attr.type_name),
            attr.location});
        }

        return attributes;
      }
      catch (const std::exception &e)
      {
        // Re-throw with more context
        throw std::runtime_error(std::string("Failed to parse GLSL attributes: ") + e.what());
      }
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
      try
      {
        auto location = holocron::webgl::getGLSLAttribLocation(source.c_str(), name.c_str());
        return location;
      }
      catch (const std::exception &e)
      {
        // If attribute not found, return nullopt instead of throwing
        std::string error_msg(e.what());
        if (error_msg.find("not found") != std::string::npos)
        {
          return std::nullopt;
        }
        // Re-throw parsing errors
        throw std::runtime_error(std::string("Failed to parse GLSL attributes: ") + e.what());
      }
    }
  };
}
