#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include "./common.hpp"

namespace gles
{
  class GPUCommandBufferImpl : public commandbuffers::GPUCommandBufferBase
  {
  public:
    GPUCommandBufferImpl(commandbuffers::GPUCommandEncoder *encoder,
                         const commandbuffers::GPUCommandBufferDescriptor *descriptor);

  public:
    void execute();

  private:
    void onDraw(const commandbuffers::GPUDrawCommand &);
    void onDraw(const commandbuffers::GPUDrawIndexedCommand &);
    void setViewport(const commandbuffers::GPUSetViewportCommand &);
    void setScissorRect(const commandbuffers::GPUSetScissorCommand &);

  private:
    GLuint framebuffer_;
  };
}
