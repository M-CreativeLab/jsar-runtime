#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <common/command_buffers/gpu/gpu_adapter.hpp>

namespace client_graphics
{
  /**
   * The `WebGPUDevice` class represents a logical WebGPU device.
   * This is the client-side implementation that records commands into command buffers
   * following the same pattern as WebGLContext.
   */
  class WebGPUDevice
  {
  public:
    WebGPUDevice(const commandbuffers::GPUAdapterInfo &adapter_info,
                 std::optional<std::string> label = std::nullopt);
    ~WebGPUDevice() = default;

  public:
    // Device properties
    const commandbuffers::GPUAdapterInfo &adapterInfo() const
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

    // Command recording
    std::unique_ptr<class WebGPUCommandEncoder> createCommandEncoder(
      std::optional<std::string> label = std::nullopt);

    // Resource creation (placeholder - full implementation would be in subsequent issues)
    // std::unique_ptr<class WebGPUBuffer> createBuffer(const WebGPUBufferDescriptor& descriptor);
    // std::unique_ptr<class WebGPUTexture> createTexture(const WebGPUTextureDescriptor& descriptor);
    // std::unique_ptr<class WebGPURenderPipeline> createRenderPipeline(const WebGPURenderPipelineDescriptor& descriptor);

    // Queue for command submission
    class WebGPUQueue &queue()
    {
      return *queue_;
    }

    // Error handling
    void setUncapturedErrorCallback(std::function<void(const std::string &, const std::string &)> callback);

    // Device loss handling
    bool isLost() const
    {
      return is_lost_;
    }
    void simulateLoss(); // For testing purposes

  private:
    commandbuffers::GPUAdapterInfo adapter_info_;
    commandbuffers::GPUSupportedFeatures features_;
    commandbuffers::GPUSupportedLimits limits_;
    std::unique_ptr<class WebGPUQueue> queue_;
    std::function<void(const std::string &, const std::string &)> error_callback_;
    bool is_lost_ = false;
    std::string label_;
  };
}