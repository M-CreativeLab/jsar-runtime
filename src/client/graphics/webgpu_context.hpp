#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>

// Forward declarations for WebGPU classes
namespace client_graphics
{
  class WebGPUCommandEncoder;
  class WebGPUQueue;
}

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

  /**
   * The `WebGPUQueue` class represents the command queue for a WebGPU device.
   * It handles command buffer submission and manages the command recording lifecycle.
   */
  class WebGPUQueue
  {
  public:
    WebGPUQueue(std::optional<std::string> label = std::nullopt);
    ~WebGPUQueue() = default;

  public:
    /**
     * Submit command buffers to the queue.
     * In this client-side implementation, this records the submission for later transmission.
     */
    void submit(const std::vector<std::unique_ptr<class WebGPUCommandBuffer>> &commandBuffers);

    // Write operations (placeholder for future implementation)
    // void writeBuffer(WebGPUBuffer& buffer, uint64_t offset, const void* data, size_t size);
    // void writeTexture(const WebGPUImageCopyTexture& destination, const void* data, size_t dataSize, const WebGPUImageDataLayout& dataLayout, const WebGPUExtent3D& size);

  private:
    std::string label_;
    std::vector<std::unique_ptr<class WebGPUCommandBuffer>> submitted_command_buffers_;
  };

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