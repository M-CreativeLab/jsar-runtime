#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>

using namespace std;

namespace commandbuffers
{
  GPUCommandBufferBase::GPUCommandBufferBase(GPUCommandEncoder *encoder,
                                             const GPUCommandBufferDescriptor *descriptor)
      : GPUHandle(encoder->device(), descriptor->label)
  {
  }

  GPUCommandBufferBase::GPUCommandBufferBase(Ref<GPUDeviceBase> device, GPUHandle::ErrorTag tag, string_view label)
      : GPUHandle(device, tag, label)
  {
  }

  const string &GPUCommandBufferBase::getEncoderLabel() const
  {
    return encoder_label_;
  }

  void GPUCommandBufferBase::setEncoderLabel(string encoderLabel)
  {
    encoder_label_ = move(encoderLabel);
  }

  const gpu::CommandBufferResourceUsage &GPUCommandBufferBase::getResourceUsages() const
  {
    return resource_usages_;
  }

  const vector<gpu::IndirectDrawMetadata> &GPUCommandBufferBase::getIndirectDrawMetadata()
  {
    return indirect_draw_metadata_;
  }
}
