#pragma once

#include <string>
#include <vector>

namespace client_graphics
{
  /**
   * Context attributes for WebGPU context creation.
   * Based on GPUCanvasConfiguration from WebGPU specification.
   */
  class WebGPUContextAttributes
  {
  public:
    std::string powerPreference = "default"; // "low-power" | "high-performance" | "default"
    bool forceFallbackAdapter = false;

    // Canvas configuration
    std::string format = "bgra8unorm";
    std::vector<std::string> viewFormats;
    std::string colorSpace = "srgb";
    std::string alphaMode = "opaque";
  };
}