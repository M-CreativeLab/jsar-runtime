#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>

#include "./common.hpp"
#include "./gpu_renderpass_encoder_impl.hpp"

namespace gles
{
  class GPUCommandBufferImpl : public commandbuffers::GPUCommandBuffer
  {
  public:
    GPUCommandBufferImpl(std::optional<std::string> label);
    GPUCommandBufferImpl(std::optional<std::string> label, const GPURenderPassEncoderImpl &);

  public:
    void execute() override;

  private:
    void onDraw(const commandbuffers::GPUDrawCommand &);
    void onDraw(const commandbuffers::GPUDrawIndexedCommand &);
    void setViewport(const commandbuffers::GPUSetViewportCommand &);
    void setScissorRect(const commandbuffers::GPUSetScissorCommand &);

  private:
    GLuint framebuffer_;
  };
}
