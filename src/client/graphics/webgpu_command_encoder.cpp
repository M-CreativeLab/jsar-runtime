#include "./webgpu_command_encoder.hpp"

#include <iostream>
#include <stdexcept>

namespace client_graphics
{
  using namespace std;

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