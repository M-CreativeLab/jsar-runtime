#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "./webgpu_context_attributes.hpp"
#include "./webgpu_adapter.hpp"

namespace client_graphics
{
  /**
   * The `WebGPUContext` class provides the main interface for WebGPU functionality.
   * This is analogous to WebGLContext but for the WebGPU API.
   */
  class WebGPUContext
  {
  public:
    WebGPUContext(const WebGPUContextAttributes &attrs = WebGPUContextAttributes{});
    ~WebGPUContext() = default;

  public:
    // Adapter and device management
    std::vector<std::unique_ptr<WebGPUAdapter>> enumerateAdapters();
    std::unique_ptr<WebGPUAdapter> requestAdapter(
      std::optional<std::string> powerPreference = std::nullopt,
      bool forceFallbackAdapter = false);

    // Canvas configuration (for future canvas integration)
    void configure(const WebGPUContextAttributes &configuration);
    WebGPUContextAttributes getConfiguration() const
    {
      return attributes_;
    }

    // Context state
    bool isConfigured() const
    {
      return configured_;
    }

    // Error handling
    void setUncapturedErrorCallback(std::function<void(const std::string &, const std::string &)> callback);

  private:
    WebGPUContextAttributes attributes_;
    bool configured_ = false;
    std::function<void(const std::string &, const std::string &)> error_callback_;
  };
}