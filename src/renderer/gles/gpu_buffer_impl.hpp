#pragma once

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <renderer/gles/common.hpp>

namespace gles
{
  class GPUBufferImpl : public commandbuffers::GPUBufferBase
  {
  public:
    GPUBufferImpl(Ref<commandbuffers::GPUDeviceBase> device,
                  const commandbuffers::GPUBufferDescriptor &descriptor);
    ~GPUBufferImpl();

    commandbuffers::gpu::MaybeError uploadData(uint64_t bufferOffset,
                                               const void *data,
                                               size_t size) override;

  private:
    GLenum target_;
    GLuint buffer_;
  };
}
