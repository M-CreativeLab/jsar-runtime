#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <common/command_buffers/gpu/gpu_adapter.hpp>

namespace client_graphics
{
  /**
   * The `WebGPUAdapter` class represents a WebGPU adapter which provides information
   * about the underlying graphics hardware and its capabilities.
   */
  class WebGPUAdapter
  {
  public:
    WebGPUAdapter(const commandbuffers::GPUAdapterInfo &info);
    ~WebGPUAdapter() = default;

  public:
    const commandbuffers::GPUAdapterInfo &info() const
    {
      return adapter_info_;
    }
    const commandbuffers::GPUSupportedFeatures &features() const
    {
      return features_;
    }
    const commandbuffers::GPUSupportedLimits &limits() const
    {
      return limits_;
    }

    /**
     * Request a device from this adapter.
     * @param label Optional label for the device
     * @param requiredFeatures Features required by the device
     * @param requiredLimits Limits required by the device
     * @returns A WebGPU device or nullptr if device creation fails
     */
    std::unique_ptr<class WebGPUDevice> requestDevice(
      std::optional<std::string> label = std::nullopt,
      const std::vector<std::string> &requiredFeatures = {},
      const std::unordered_map<std::string, uint32_t> &requiredLimits = {});

  private:
    commandbuffers::GPUAdapterInfo adapter_info_;
    commandbuffers::GPUSupportedFeatures features_;
    commandbuffers::GPUSupportedLimits limits_;
  };
}