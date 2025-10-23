#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>
#include "./webgpu_buffer.hpp"
#include "./webgpu_bind_group.hpp"
#include "./webgpu_render_pipeline.hpp"

namespace client_graphics
{
  /**
   * The `WebGPURenderPassEncoder` class allows recording of render commands within a render pass.
   * This extends the common GPU render pass encoder with client-side specific functionality.
   */
  class WebGPURenderPassEncoder
  {
  public:
    WebGPURenderPassEncoder(std::shared_ptr<class WebGPUCommandBuffer> commandBuffer,
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
    std::shared_ptr<class WebGPUCommandBuffer> command_buffer_;
    commandbuffers::GPURenderPassDescriptor descriptor_;
    std::string label_;
    bool ended_ = false;
    uint32_t debug_group_depth_ = 0;
  };
}