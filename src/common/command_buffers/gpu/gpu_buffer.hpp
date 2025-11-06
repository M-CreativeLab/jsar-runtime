#pragma once

#include <atomic>
#include <string_view>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  enum class GPUBufferUsage : uint64_t
  {
    kNone,
    kMapRead,
    kMapWrite,
    kCopySrc,
    kCopyDst,
    kIndex,
    kVertex,
    kUniform,
    kStorage,
    kIndirect,
    kQueryResolve,
    kTexelBuffer,
  };

  struct GPUBufferDescriptor
  {
    std::string_view label;
    GPUBufferUsage usage = GPUBufferUsage::kNone;
    uint64_t size;
    bool mappedAtCreation = false;

    // Equality operators, mostly for testing. Note that this tests
    // strict pointer-pointer equality if the struct contains member pointers.
    bool operator==(const GPUBufferDescriptor &rhs) const;
  };

  class GPUBufferBase : public GPUHandle
  {
  public:
    enum class BufferState
    {
      kUnmapped,
      kPendingMap,
      kMapped,
      kMappedAtCreation,
      kHostMappedPersistent,
      kSharedMemoryNoAccess,
      kDestroyed,
    };
    static bool IsMappedState(BufferState state)
    {
      return state == BufferState::kMapped ||
             state == BufferState::kMappedAtCreation ||
             state == BufferState::kHostMappedPersistent;
    }

  public:
    virtual ~GPUBufferBase() = default;

    GPUHandleType type() const override final
    {
      return GPUHandleType::kBuffer;
    }

  public:
    size_t size() const;
    GPUBufferUsage usage() const;
    BufferState mapState() const;
    void unmap();

  protected:
    GPUBufferBase(Ref<GPUDeviceBase> device, const GPUBufferDescriptor &descriptor);
    GPUBufferBase(Ref<GPUDeviceBase> device,
                  const GPUBufferDescriptor *descriptor,
                  GPUHandle::ErrorTag tag);

    bool mapAtCreationInternal();

    BufferState state() const;
    GPUMapMode mapMode() const;
    size_t mapOffset() const;
    size_t mapSize() const;

    uint64_t mAllocatedSize = 0;

  private:
    size_t size_ = 0;
    bool is_data_initialized = false;

    GPUBufferUsage usage_ = GPUBufferUsage::kNone;
    GPUBufferUsage internal_usage_ = GPUBufferUsage::kNone;
    std::atomic<BufferState> map_state_ = BufferState::kUnmapped;

    // A recursive buffer used to implement mappedAtCreation for buffers with non-mappable usage.
    Ref<GPUBufferBase> staging_buffer_ = nullptr;

    // Mapping specific states.
    GPUMapMode mMapMode = GPUMapMode::kNone;
    size_t mMapOffset = 0;
    size_t mMapSize = 0;
  };
}
