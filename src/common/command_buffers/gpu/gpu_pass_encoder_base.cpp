#include <common/command_buffers/gpu/gpu_pass_encoder_base.hpp>

namespace commandbuffers
{
  GPUPassEncoderBase::GPUPassEncoderBase()
      : ended_(false)
  {
  }

  void GPUPassEncoderBase::begin()
  {
    ended_ = false;
    assert(command_buffer_ != nullptr && "Command buffer must be initialized before beginning pass encoder.");
  }

  void GPUPassEncoderBase::end()
  {
    ended_ = true;
  }

  const GPUCommandBufferBase &GPUPassEncoderBase::commandBuffer() const
  {
    assert(command_buffer_ != nullptr && "Command buffer is not initialized.");
    return *command_buffer_;
  }
}
