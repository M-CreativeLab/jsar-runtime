#include "./webgpu_command_encoder.hpp"

#include <iostream>
#include <stdexcept>

namespace client_graphics
{
  using namespace std;

  // WebGPUCommandBuffer implementation
  WebGPUCommandBuffer::WebGPUCommandBuffer(std::optional<std::string> label)
      : label_(label.value_or("WebGPUCommandBuffer"))
  {
  }

  void WebGPUCommandBuffer::execute() const
  {
    // In the client-side implementation, this would typically transmit
    // the recorded commands to the graphics server for execution.
    // For now, we just log the execution for debugging purposes.

    if (commands_.empty())
    {
      return;
    }

    // In a full implementation, this would serialize and send the commands
    // to the server-side renderer via the command buffer system.
    // Following the WebGL pattern in webgl_context.cpp

    std::cout << "Executing WebGPU command buffer '" << label_
              << "' with " << commands_.size() << " commands" << std::endl;

    // Placeholder for command transmission logic
    // This would follow the same pattern as WebGLContext::sendCommandBufferRequest()
  }

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

  // WebGPUCommandEncoder implementation
  WebGPUCommandEncoder::WebGPUCommandEncoder(std::optional<std::string> label)
      : command_buffer_(std::make_shared<WebGPUCommandBuffer>(label))
      , label_(label.value_or("WebGPUCommandEncoder"))
  {
  }

  std::unique_ptr<WebGPURenderPassEncoder> WebGPUCommandEncoder::beginRenderPass(
    const commandbuffers::GPURenderPassDescriptor &descriptor)
  {
    if (finished_)
    {
      throw std::runtime_error("Cannot begin render pass on finished command encoder");
    }

    // Create render pass encoder with shared command buffer
    auto encoder = std::make_unique<WebGPURenderPassEncoder>(
      command_buffer_, descriptor, "RenderPass");

    std::cout << "WebGPU: Begin render pass in encoder '" << label_ << "'" << std::endl;

    return encoder;
  }

  void WebGPUCommandEncoder::copyBufferToBuffer(const WebGPUBuffer &source,
                                                uint64_t sourceOffset,
                                                const WebGPUBuffer &destination,
                                                uint64_t destinationOffset,
                                                uint64_t size)
  {
    if (finished_)
    {
      throw std::runtime_error("Cannot copy buffer on finished command encoder");
    }

    // Record buffer copy command
    // In a full implementation, this would use actual buffer objects
    std::cout << "WebGPU: Copy buffer to buffer (" << size << " bytes) in encoder '" << label_ << "'" << std::endl;
  }

  void WebGPUCommandEncoder::clearBuffer(const WebGPUBuffer &buffer,
                                         uint64_t offset,
                                         uint64_t size)
  {
    if (finished_)
    {
      throw std::runtime_error("Cannot clear buffer on finished command encoder");
    }

    // Record buffer clear command
    std::cout << "WebGPU: Clear buffer (" << size << " bytes at offset " << offset
              << ") in encoder '" << label_ << "'" << std::endl;
  }

  std::unique_ptr<WebGPUCommandBuffer> WebGPUCommandEncoder::finish(
    std::optional<std::string> label)
  {
    if (finished_)
    {
      throw std::runtime_error("Command encoder already finished");
    }

    if (debug_group_depth_ > 0)
    {
      throw std::runtime_error("Cannot finish command encoder with unclosed debug groups");
    }

    finished_ = true;

    // Create a new command buffer with the recorded commands
    auto finalLabel = label.value_or(command_buffer_->label());
    auto finishedBuffer = std::make_unique<WebGPUCommandBuffer>(finalLabel);

    // Transfer commands from shared buffer to the finished buffer
    finishedBuffer->commands_ = std::move(command_buffer_->commands_);

    std::cout << "WebGPU: Finish command encoder '" << label_
              << "' with " << finishedBuffer->commandCount() << " commands" << std::endl;

    return finishedBuffer;
  }

  void WebGPUCommandEncoder::pushDebugGroup(const std::string &groupLabel)
  {
    if (finished_)
    {
      throw std::runtime_error("Cannot push debug group on finished command encoder");
    }

    debug_group_depth_++;
    std::cout << "WebGPU: Push debug group '" << groupLabel << "' in encoder '" << label_ << "'" << std::endl;
  }

  void WebGPUCommandEncoder::popDebugGroup()
  {
    if (finished_)
    {
      throw std::runtime_error("Cannot pop debug group on finished command encoder");
    }

    if (debug_group_depth_ == 0)
    {
      throw std::runtime_error("No debug group to pop");
    }

    debug_group_depth_--;
    std::cout << "WebGPU: Pop debug group in encoder '" << label_ << "'" << std::endl;
  }

  void WebGPUCommandEncoder::insertDebugMarker(const std::string &markerLabel)
  {
    if (finished_)
    {
      throw std::runtime_error("Cannot insert debug marker on finished command encoder");
    }

    std::cout << "WebGPU: Debug marker '" << markerLabel << "' in encoder '" << label_ << "'" << std::endl;
  }
}