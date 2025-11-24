#pragma once

#include <string>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_queue.hpp>

namespace gles
{
  class GPUQueueImpl : public commandbuffers::GPUQueueBase
  {
  public:
    static commandbuffers::gpu::ResultOrError<Ref<GPUQueueImpl>> Create(
      Ref<commandbuffers::GPUDeviceBase> device,
      const commandbuffers::GPUQueueDescriptor &descriptor);

  private:
    GPUQueueImpl(Ref<commandbuffers::GPUDeviceBase> device,
                 const commandbuffers::GPUQueueDescriptor &descriptor);

    commandbuffers::gpu::MaybeError submitImpl(
      uint32_t commandCount,
      commandbuffers::GPUCommandBufferBase *const *commands) override;
    commandbuffers::gpu::MaybeError writeBufferImpl(
      commandbuffers::GPUBufferBase *buffer,
      uint64_t bufferOffset,
      const void *data,
      size_t size) override;

    bool hasPendingCommands() const override;
    commandbuffers::gpu::MaybeError waitForIdleForDestructionImpl() override;

    uint32_t egl_sync_type_;
    bool has_pending_commands_ = false;
  };
}
