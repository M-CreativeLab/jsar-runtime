#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>
#include "./webgpu_resources.hpp"

// Forward declarations
namespace client_graphics
{
  class WebGPUCommandBuffer;
  class WebGPURenderPassEncoder;
  class WebGPUCommandEncoder;
}

namespace client_graphics
{
  /**
   * The `WebGPUCommandBuffer` class represents a recorded sequence of GPU commands.
   * This is the client-side implementation that holds recorded commands for later submission.
   */
  class WebGPUCommandBuffer
  {
  public:
    WebGPUCommandBuffer(std::optional<std::string> label = std::nullopt);
    ~WebGPUCommandBuffer() = default;

    // Non-copyable but movable
    WebGPUCommandBuffer(const WebGPUCommandBuffer &) = delete;
    WebGPUCommandBuffer &operator=(const WebGPUCommandBuffer &) = delete;
    WebGPUCommandBuffer(WebGPUCommandBuffer &&) = default;
    WebGPUCommandBuffer &operator=(WebGPUCommandBuffer &&) = default;

  public:
    const std::string &label() const
    {
      return label_;
    }
    bool isEmpty() const
    {
      return commands_.empty();
    }
    size_t commandCount() const
    {
      return commands_.size();
    }

    /**
     * Execute the recorded commands. In the client-side implementation,
     * this would typically transmit the commands to the server.
     * For now, this is a placeholder for the command recording pattern.
     */
    void execute() const;

  private:
    friend class WebGPUCommandEncoder;
    friend class WebGPURenderPassEncoder;

    std::string label_;
    std::vector<std::shared_ptr<commandbuffers::GPUCommand>> commands_;

    /**
     * Add a command to the command buffer.
     * This follows the same pattern as the existing GPU command buffer implementation.
     */
    template <typename T, typename... Args>
    void addCommand(Args &&...args)
    {
      auto command = std::make_shared<T>(std::forward<Args>(args)...);
      commands_.push_back(command);
    }
  };

  /**
   * The `WebGPURenderPassEncoder` class allows recording of render commands within a render pass.
   * This extends the common GPU render pass encoder with client-side specific functionality.
   */
  class WebGPURenderPassEncoder
  {
  public:
    WebGPURenderPassEncoder(std::shared_ptr<WebGPUCommandBuffer> commandBuffer,
                            const commandbuffers::GPURenderPassDescriptor &descriptor,
                            std::optional<std::string> label = std::nullopt);
    ~WebGPURenderPassEncoder() = default;

    // Non-copyable, non-movable (following WebGPU specification)
    WebGPURenderPassEncoder(const WebGPURenderPassEncoder &) = delete;
    WebGPURenderPassEncoder &operator=(const WebGPURenderPassEncoder &) = delete;
    WebGPURenderPassEncoder(WebGPURenderPassEncoder &&) = delete;
    WebGPURenderPassEncoder &operator=(WebGPURenderPassEncoder &&) = delete;

  public:
    // Render state management
    void setPipeline(const class WebGPURenderPipeline &pipeline);
    void setBindGroup(uint32_t index, const class WebGPUBindGroup &bindGroup, const std::vector<uint32_t> &dynamicOffsets = {});

    // Vertex and index data
    void setVertexBuffer(uint32_t slot, const class WebGPUBuffer &buffer, uint64_t offset = 0, uint64_t size = 0);
    void setIndexBuffer(const class WebGPUBuffer &buffer,
                        commandbuffers::GPUIndexFormat format,
                        uint64_t offset = 0,
                        uint64_t size = 0);

    // Drawing commands
    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t baseVertex = 0, uint32_t firstInstance = 0);

    // Render state commands
    void setViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
    void setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    // Pass management
    void end();
    bool isEnded() const
    {
      return ended_;
    }

    // Debug commands
    void pushDebugGroup(const std::string &groupLabel);
    void popDebugGroup();
    void insertDebugMarker(const std::string &markerLabel);

  private:
    std::shared_ptr<WebGPUCommandBuffer> command_buffer_;
    commandbuffers::GPURenderPassDescriptor descriptor_;
    std::string label_;
    bool ended_ = false;
    uint32_t debug_group_depth_ = 0;
  };

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
    void copyBufferToBuffer(const class WebGPUBuffer &source, uint64_t sourceOffset, const class WebGPUBuffer &destination, uint64_t destinationOffset, uint64_t size);

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
    void clearBuffer(const class WebGPUBuffer &buffer, uint64_t offset = 0, uint64_t size = 0);

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