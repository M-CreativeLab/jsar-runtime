#pragma once

#include <memory>
#include <optional>
#include <string>

#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>
#include "./webgpu_command_buffer.hpp"
#include "./webgpu_render_pass_encoder.hpp"
#include "./webgpu_buffer.hpp"

namespace client_graphics
{
  /**
   * The `WebGPUCommandEncoder` class allows recording of GPU commands.
   * This is the client-side implementation that records commands into command buffers.
   */
  class WebGPUCommandEncoder
  {
  public:
    WebGPUCommandEncoder(std::optional<std::string> label = std::nullopt);
    ~WebGPUCommandEncoder() = default;

    // Non-copyable, non-movable (following WebGPU specification)
    WebGPUCommandEncoder(const WebGPUCommandEncoder &) = delete;
    WebGPUCommandEncoder &operator=(const WebGPUCommandEncoder &) = delete;
    WebGPUCommandEncoder(WebGPUCommandEncoder &&) = delete;
    WebGPUCommandEncoder &operator=(WebGPUCommandEncoder &&) = delete;

  public:
    /**
     * Begin a render pass.
     * @param descriptor Render pass configuration
     * @returns A render pass encoder for recording render commands
     */
    std::unique_ptr<WebGPURenderPassEncoder> beginRenderPass(
      const commandbuffers::GPURenderPassDescriptor &descriptor);

    /**
     * Begin a compute pass (placeholder for future implementation).
     * @param descriptor Compute pass configuration
     * @returns A compute pass encoder for recording compute commands
     */
    // std::unique_ptr<WebGPUComputePassEncoder> beginComputePass(
    //   const WebGPUComputePassDescriptor& descriptor = {});

    /**
     * Copy buffer to buffer.
     * @param source Source buffer
     * @param sourceOffset Offset in source buffer
     * @param destination Destination buffer
     * @param destinationOffset Offset in destination buffer
     * @param size Number of bytes to copy
     */
    void copyBufferToBuffer(const WebGPUBuffer &source, uint64_t sourceOffset, const WebGPUBuffer &destination, uint64_t destinationOffset, uint64_t size);

    /**
     * Copy buffer to texture (placeholder for future implementation).
     */
    // void copyBufferToTexture(const WebGPUImageCopyBuffer& source,
    //                         const WebGPUImageCopyTexture& destination,
    //                         const WebGPUExtent3D& copySize);

    /**
     * Copy texture to buffer (placeholder for future implementation).
     */
    // void copyTextureToBuffer(const WebGPUImageCopyTexture& source,
    //                         const WebGPUImageCopyBuffer& destination,
    //                         const WebGPUExtent3D& copySize);

    /**
     * Copy texture to texture (placeholder for future implementation).
     */
    // void copyTextureToTexture(const WebGPUImageCopyTexture& source,
    //                          const WebGPUImageCopyTexture& destination,
    //                          const WebGPUExtent3D& copySize);

    /**
     * Clear buffer with zeros.
     * @param buffer Buffer to clear
     * @param offset Offset to start clearing from
     * @param size Number of bytes to clear
     */
    void clearBuffer(const WebGPUBuffer &buffer, uint64_t offset = 0, uint64_t size = 0);

    /**
     * Resolve query set (placeholder for future implementation).
     */
    // void resolveQuerySet(const WebGPUQuerySet& querySet, uint32_t firstQuery, uint32_t queryCount,
    //                     const WebGPUBuffer& destination, uint64_t destinationOffset);

    /**
     * Finish recording and return the command buffer.
     * @param label Optional label for the command buffer
     * @returns The recorded command buffer
     */
    std::unique_ptr<WebGPUCommandBuffer> finish(std::optional<std::string> label = std::nullopt);

    // Debug commands
    void pushDebugGroup(const std::string &groupLabel);
    void popDebugGroup();
    void insertDebugMarker(const std::string &markerLabel);

  private:
    std::shared_ptr<WebGPUCommandBuffer> command_buffer_;
    std::string label_;
    bool finished_ = false;
    uint32_t debug_group_depth_ = 0;
  };
}