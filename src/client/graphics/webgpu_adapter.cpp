#include "./webgpu_adapter.hpp"
#include "./webgpu_device.hpp"

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
}