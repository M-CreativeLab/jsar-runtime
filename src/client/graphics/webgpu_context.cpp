#include "./webgpu_context.hpp"
#include "./webgpu_adapter.hpp"

#include <iostream>
#include <stdexcept>

namespace client_graphics
{
  using namespace std;

  // WebGPUContext implementation
  WebGPUContext::WebGPUContext(const WebGPUContextAttributes &attrs)
      : attributes_(attrs)
  {
    // Context initialization
    // In a full implementation, this would set up communication with the graphics server
  }

  std::vector<std::unique_ptr<WebGPUAdapter>> WebGPUContext::enumerateAdapters()
  {
    std::vector<std::unique_ptr<WebGPUAdapter>> adapters;

    // Create a default adapter for this client-side implementation
    commandbuffers::GPUAdapterInfo defaultAdapterInfo;
    defaultAdapterInfo.vendor = "JSAR Runtime";
    defaultAdapterInfo.description = "Default WebGPU Adapter";
    defaultAdapterInfo.device = "Software Renderer";
    defaultAdapterInfo.architecture = "Generic";
    defaultAdapterInfo.subgroupMaxSize = 64;
    defaultAdapterInfo.subgroupMinSize = 4;

    adapters.push_back(std::make_unique<WebGPUAdapter>(defaultAdapterInfo));

    return adapters;
  }

  std::unique_ptr<WebGPUAdapter> WebGPUContext::requestAdapter(
    std::optional<std::string> powerPreference,
    bool forceFallbackAdapter)
  {
    // For this client-side implementation, return the default adapter
    auto adapters = enumerateAdapters();
    if (!adapters.empty())
    {
      return std::move(adapters[0]);
    }

    return nullptr;
  }

  void WebGPUContext::configure(const WebGPUContextAttributes &configuration)
  {
    attributes_ = configuration;
    configured_ = true;
  }

  void WebGPUContext::setUncapturedErrorCallback(
    std::function<void(const std::string &, const std::string &)> callback)
  {
    error_callback_ = std::move(callback);
  }
}