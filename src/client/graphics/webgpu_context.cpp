#include "./webgpu_context.hpp"
#include "./webgpu_command_encoder.hpp"

#include <iostream>
#include <stdexcept>

namespace client_graphics
{
  using namespace std;

  // WebGPUAdapter implementation
  WebGPUAdapter::WebGPUAdapter(const commandbuffers::GPUAdapterInfo &info)
      : adapter_info_(info)
      , features_()
      , limits_()
  {
    // Initialize with default features and limits
    // In a full implementation, these would be queried from the actual adapter
  }

  std::unique_ptr<WebGPUDevice> WebGPUAdapter::requestDevice(
    std::optional<std::string> label,
    const std::vector<std::string> &requiredFeatures,
    const std::unordered_map<std::string, uint32_t> &requiredLimits)
  {
    // Validate required features
    for (const auto &feature : requiredFeatures)
    {
      if (features_.find(feature) == features_.end())
      {
        // Feature not supported
        return nullptr;
      }
    }

    // Validate required limits
    for (const auto &[limitName, requiredValue] : requiredLimits)
    {
      auto it = limits_.find(limitName);
      if (it == limits_.end() || it->second < requiredValue)
      {
        // Limit not supported or insufficient
        return nullptr;
      }
    }

    // Create device with the requested configuration
    return std::make_unique<WebGPUDevice>(adapter_info_, label);
  }

  // WebGPUDevice implementation
  WebGPUDevice::WebGPUDevice(const commandbuffers::GPUAdapterInfo &adapter_info,
                             std::optional<std::string> label)
      : adapter_info_(adapter_info)
      , features_()
      , limits_()
      , queue_(std::make_unique<WebGPUQueue>("DefaultQueue"))
      , label_(label.value_or("WebGPUDevice"))
  {
    // Device initialization would typically involve creating actual GPU resources
    // For this client-side implementation, we just set up the command recording infrastructure
  }

  std::unique_ptr<WebGPUCommandEncoder> WebGPUDevice::createCommandEncoder(
    std::optional<std::string> label)
  {
    if (is_lost_)
    {
      if (error_callback_)
      {
        error_callback_("device-lost", "Cannot create command encoder on lost device");
      }
      return nullptr;
    }

    return std::make_unique<WebGPUCommandEncoder>(label);
  }

  void WebGPUDevice::setUncapturedErrorCallback(
    std::function<void(const std::string &, const std::string &)> callback)
  {
    error_callback_ = std::move(callback);
  }

  void WebGPUDevice::simulateLoss()
  {
    is_lost_ = true;
    if (error_callback_)
    {
      error_callback_("device-lost", "Device lost");
    }
  }

  // WebGPUQueue implementation
  WebGPUQueue::WebGPUQueue(std::optional<std::string> label)
      : label_(label.value_or("WebGPUQueue"))
  {
  }

  void WebGPUQueue::submit(const std::vector<std::unique_ptr<WebGPUCommandBuffer>> &commandBuffers)
  {
    // In the client-side implementation, we record the submission
    // In a full implementation, this would transmit commands to the server
    for (const auto &commandBuffer : commandBuffers)
    {
      if (commandBuffer && !commandBuffer->isEmpty())
      {
        // Execute the command buffer (placeholder)
        commandBuffer->execute();
      }
    }
  }

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