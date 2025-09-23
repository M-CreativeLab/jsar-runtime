#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <common/command_buffers/gpu/gpu_adapter.hpp>

namespace client_graphics
{
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
}