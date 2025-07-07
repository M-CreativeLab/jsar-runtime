#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>
#include "./common.hpp"

namespace gles
{
  class GPURenderPassEncoderImpl : public commandbuffers::GPURenderPassEncoder
  {
    friend class GPUCommandBufferImpl;

  public:
    GPURenderPassEncoderImpl(std::string label = "GPURenderPassEncoder");

    void begin() override;
    GLuint targetFramebuffer() const
    {
      return framebuffer_;
    }

  private:
    GLuint framebuffer_;
  };
}
