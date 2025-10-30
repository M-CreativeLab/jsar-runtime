#pragma once

#include <cassert>
#include <memory>
#include "./gpu_command_buffer.hpp"

namespace commandbuffers
{
  class GPUPassEncoderBase
  {
  public:
    GPUPassEncoderBase();
    virtual ~GPUPassEncoderBase() = default;

  public:
    virtual bool isComputePassEncoder() const
    {
      return false;
    }
    virtual bool isRenderPassEncoder() const
    {
      return false;
    }

    virtual void begin();
    virtual void end();
    const GPUCommandBufferBase &commandBuffer() const;

  protected:
    std::unique_ptr<GPUCommandBufferBase> command_buffer_;
    bool ended_ = false;
  };
}
