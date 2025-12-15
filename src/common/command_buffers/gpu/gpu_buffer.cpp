#include <common/command_buffers/gpu/gpu_buffer.hpp>

namespace commandbuffers
{
  bool GPUBufferDescriptor::operator==(const GPUBufferDescriptor &rhs) const
  {
    return label == rhs.label && usage == rhs.usage && size == rhs.size && mappedAtCreation == rhs.mappedAtCreation;
  }

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

  GPUBufferBase::GPUBufferBase(Ref<GPUDeviceBase> device, const GPUBufferDescriptor &descriptor)
      : GPUHandle(device, descriptor.label)
  {
    size_ = static_cast<size_t>(descriptor.size);
    usage_ = descriptor.usage;
    map_state_.store(descriptor.mappedAtCreation ? BufferState::kMappedAtCreation : BufferState::kUnmapped);
    if (descriptor.mappedAtCreation)
    {
      mMapMode = GPUMapMode::kWrite;
      mMapOffset = 0;
      mMapSize = static_cast<size_t>(descriptor.size);
    }
  }

  GPUBufferBase::GPUBufferBase(Ref<GPUDeviceBase> device,
                               const GPUBufferDescriptor *descriptor,
                               GPUHandle::ErrorTag tag)
      : GPUHandle(device, tag, descriptor ? descriptor->label : std::string_view{})
  {
    if (descriptor)
    {
      size_ = static_cast<size_t>(descriptor->size);
      usage_ = descriptor->usage;
      map_state_.store(descriptor->mappedAtCreation ? BufferState::kMappedAtCreation : BufferState::kUnmapped);
    }
  }

  bool GPUBufferBase::mapAtCreationInternal()
  {
    map_state_.store(BufferState::kMappedAtCreation);
    mMapMode = GPUMapMode::kWrite;
    mMapOffset = 0;
    mMapSize = size_;
    return true;
  }

  GPUBufferBase::BufferState GPUBufferBase::state() const
  {
    return map_state_.load();
  }

  GPUMapMode GPUBufferBase::mapMode() const
  {
    return mMapMode;
  }

  size_t GPUBufferBase::mapOffset() const
  {
    return mMapOffset;
  }

  size_t GPUBufferBase::mapSize() const
  {
    return mMapSize;
  }
}
