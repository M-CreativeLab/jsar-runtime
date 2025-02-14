#pragma once

#include <iostream>
#include <concepts>
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
}
