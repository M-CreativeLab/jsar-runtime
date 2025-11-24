#include <common/command_buffers/gpu/gpu_buffer.hpp>

namespace commandbuffers
{
  gpu::MaybeError GPUBufferBase::uploadData(uint64_t bufferOffset, const void *data, size_t size)
  {
    if (size == 0)
    {
      return {};
    }

    // TODO: implement uploadData
    return {};
  }
}
