#pragma once

#include <atomic>
#include "./gpu_base.hpp"

namespace commandbuffers
{
  enum class BufferUsage : uint64_t
  {
    None,
    MapRead,
    MapWrite,
    CopySrc,
    CopyDst,
    Index,
    Vertex,
    Uniform,
    Storage,
    Indirect,
    QueryResolve,
    TexelBuffer,
  };

  class GPUBufferBase : public GPUHandle
  {
  public:
    enum class BufferState
    {
      Unmapped,
      PendingMap,
      Mapped,
      MappedAtCreation,
      HostMappedPersistent,
      SharedMemoryNoAccess,
      Destroyed,
    };
    static bool IsMappedState(BufferState state)
    {
      return state == BufferState::Mapped ||
             state == BufferState::MappedAtCreation ||
             state == BufferState::HostMappedPersistent;
    }

  public:
    virtual ~GPUBufferBase() = default;

  public:
    size_t size() const
    {
      return size_;
    }
    BufferUsage usage() const
    {
      return usage_;
    }
    BufferState mapState() const
    {
      return map_state_.load();
    }

  private:
    size_t size_ = 0;
    BufferUsage usage_ = BufferUsage::None;
    std::atomic<BufferState> map_state_ = BufferState::Unmapped;
  };
}
