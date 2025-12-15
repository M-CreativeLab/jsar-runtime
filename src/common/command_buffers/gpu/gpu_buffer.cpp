#include <common/command_buffers/gpu/gpu_buffer.hpp>

namespace commandbuffers
{
  void GPUBufferBase::unmap()
  {
    map_state_.store(BufferState::kUnmapped);
    mMapMode = GPUMapMode::kNone;
    mMapOffset = 0;
    mMapSize = 0;
  }

  size_t GPUBufferBase::size() const
  {
    return size_;
  }

  GPUBufferUsage GPUBufferBase::usage() const
  {
    return usage_;
  }

  GPUBufferBase::BufferState GPUBufferBase::mapState() const
  {
    return map_state_.load();
  }

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
