#include <common/command_buffers/gpu/gpu_command_buffer.hpp>

using namespace std;

namespace commandbuffers
{
  GPUCommandBufferBase::GPUCommandBufferBase(GPUCommandEncoderBase *encoder,
                                             const GPUCommandBufferDescriptor *descriptor)
      : GPUHandle(encoder->device(), descriptor->label)
  {
  }

  GPUCommandBufferBase::GPUCommandBufferBase(shared_ptr<GPUDeviceBase> device,
                                             GPUHandle::ErrorTag tag,
                                             string_view label)
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

  const GPUCommandBufferResourceUsage &GPUCommandBufferBase::getResourceUsages() const
  {
    return resource_usages_;
  }

  const vector<GPUIndirectDrawMetadata> &GPUCommandBufferBase::getIndirectDrawMetadata()
  {
    return indirect_draw_metadata_;
  }
}
