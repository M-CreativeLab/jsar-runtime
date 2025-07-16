#pragma once

#include <cassert>
#include <memory>
#include "./gpu_command_buffer.hpp"

namespace commandbuffers
{
  class GPUPassEncoderBase
  {
  public:
    GPUPassEncoderBase()
        : ended_(false)
    {
    }
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

    virtual void begin()
    {
      ended_ = false;
      assert(command_buffer_ != nullptr && "Command buffer must be initialized before beginning pass encoder.");
    }
    virtual void end()
    {
      ended_ = true;
    }

    const GPUCommandBuffer &commandBuffer() const
    {
      assert(command_buffer_ != nullptr && "Command buffer is not initialized.");
      return *command_buffer_;
    }

  protected:
    std::unique_ptr<GPUCommandBuffer> command_buffer_;
    bool ended_ = false;
  };
}
