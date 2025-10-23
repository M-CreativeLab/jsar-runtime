#include "./webgpu_render_pass_encoder.hpp"
#include "./webgpu_command_buffer.hpp"

#include <iostream>
#include <stdexcept>

namespace client_graphics
{
  using namespace std;

  // WebGPURenderPassEncoder implementation
  WebGPURenderPassEncoder::WebGPURenderPassEncoder(
    std::shared_ptr<WebGPUCommandBuffer> commandBuffer,
    const commandbuffers::GPURenderPassDescriptor &descriptor,
    std::optional<std::string> label)
      : command_buffer_(std::move(commandBuffer))
      , descriptor_(descriptor)
      , label_(label.value_or("WebGPURenderPassEncoder"))
  {
    if (!command_buffer_)
    {
      throw std::runtime_error("WebGPURenderPassEncoder: Command buffer cannot be null");
    }
  }

  void WebGPURenderPassEncoder::setPipeline(const WebGPURenderPipeline &pipeline)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot set pipeline on ended render pass encoder");
    }

    // Record pipeline setting command
    // In a full implementation, this would use the actual pipeline object
    // command_buffer_->addCommand<commandbuffers::GPUSetRenderPipelineCommand>(pipeline);

    // Placeholder implementation
    std::cout << "WebGPU: Set render pipeline in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::setBindGroup(uint32_t index,
                                             const WebGPUBindGroup &bindGroup,
                                             const std::vector<uint32_t> &dynamicOffsets)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot set bind group on ended render pass encoder");
    }

    // Record bind group setting command
    // command_buffer_->addCommand<commandbuffers::GPUSetBindGroupCommand>(bindGroup, index);

    std::cout << "WebGPU: Set bind group " << index << " in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::setVertexBuffer(uint32_t slot,
                                                const WebGPUBuffer &buffer,
                                                uint64_t offset,
                                                uint64_t size)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot set vertex buffer on ended render pass encoder");
    }

    // Record vertex buffer setting command following the existing pattern
    // command_buffer_->addCommand<commandbuffers::GPUSetVertexBufferCommand>(slot, buffer, offset, size);

    std::cout << "WebGPU: Set vertex buffer slot " << slot << " in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::setIndexBuffer(const WebGPUBuffer &buffer,
                                               commandbuffers::GPUIndexFormat format,
                                               uint64_t offset,
                                               uint64_t size)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot set index buffer on ended render pass encoder");
    }

    // Record index buffer setting command
    // command_buffer_->addCommand<commandbuffers::GPUSetIndexBufferCommand>(buffer, format, offset, size);

    std::cout << "WebGPU: Set index buffer in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::draw(uint32_t vertexCount,
                                     uint32_t instanceCount,
                                     uint32_t firstVertex,
                                     uint32_t firstInstance)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot draw on ended render pass encoder");
    }

    // Record draw command using the existing GPU command structure
    command_buffer_->addCommand<commandbuffers::GPUDrawCommand>(
      vertexCount, instanceCount, firstVertex, firstInstance);

    std::cout << "WebGPU: Draw " << vertexCount << " vertices in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::drawIndexed(uint32_t indexCount,
                                            uint32_t instanceCount,
                                            uint32_t firstIndex,
                                            int32_t baseVertex,
                                            uint32_t firstInstance)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot draw indexed on ended render pass encoder");
    }

    // Record indexed draw command
    command_buffer_->addCommand<commandbuffers::GPUDrawIndexedCommand>(
      indexCount, instanceCount, firstIndex, baseVertex, firstInstance);

    std::cout << "WebGPU: Draw indexed " << indexCount << " indices in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot set viewport on ended render pass encoder");
    }

    // Record viewport command
    command_buffer_->addCommand<commandbuffers::GPUSetViewportCommand>(
      x, y, width, height, minDepth, maxDepth);

    std::cout << "WebGPU: Set viewport " << width << "x" << height << " in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::setScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot set scissor rect on ended render pass encoder");
    }

    // Record scissor command
    command_buffer_->addCommand<commandbuffers::GPUSetScissorCommand>(
      static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));

    std::cout << "WebGPU: Set scissor rect " << width << "x" << height << " in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::end()
  {
    if (ended_)
    {
      throw std::runtime_error("Render pass encoder already ended");
    }

    if (debug_group_depth_ > 0)
    {
      throw std::runtime_error("Cannot end render pass encoder with unclosed debug groups");
    }

    ended_ = true;
    std::cout << "WebGPU: End render pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::pushDebugGroup(const std::string &groupLabel)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot push debug group on ended render pass encoder");
    }

    debug_group_depth_++;
    std::cout << "WebGPU: Push debug group '" << groupLabel << "' in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::popDebugGroup()
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot pop debug group on ended render pass encoder");
    }

    if (debug_group_depth_ == 0)
    {
      throw std::runtime_error("No debug group to pop");
    }

    debug_group_depth_--;
    std::cout << "WebGPU: Pop debug group in pass '" << label_ << "'" << std::endl;
  }

  void WebGPURenderPassEncoder::insertDebugMarker(const std::string &markerLabel)
  {
    if (ended_)
    {
      throw std::runtime_error("Cannot insert debug marker on ended render pass encoder");
    }

    std::cout << "WebGPU: Debug marker '" << markerLabel << "' in pass '" << label_ << "'" << std::endl;
  }
}