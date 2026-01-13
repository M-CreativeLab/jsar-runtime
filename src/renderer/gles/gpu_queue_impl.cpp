#include <renderer/gles/gpu_queue_impl.hpp>
#include <renderer/gles/gpu_command_buffer_impl.hpp>

namespace gles
{
  using namespace std;
  using namespace commandbuffers;

  GPUQueueImpl::GPUQueueImpl(Ref<GPUDeviceBase> device,
                             const GPUQueueDescriptor &descriptor)
      : GPUQueueBase(device, descriptor)
  {
  }

  // static
  gpu::ResultOrError<Ref<GPUQueueImpl>> GPUQueueImpl::Create(Ref<GPUDeviceBase> device,
                                                             const GPUQueueDescriptor &descriptor)
  {
    assert(device != nullptr && "Failed to create GPUQueueImpl: device is null.");
    return AcquireRef(new GPUQueueImpl(device, descriptor));
  }

  gpu::MaybeError GPUQueueImpl::submitImpl(uint32_t commandCount,
                                           GPUCommandBufferBase *const *commands)
  {
    for (uint32_t i = 0; i < commandCount; ++i)
    {
      auto *cmd = static_cast<GPUCommandBufferImpl *>(commands[i]);
      cmd->execute();
    }
    return {};
  }

  gpu::MaybeError GPUQueueImpl::writeBufferImpl(GPUBufferBase *buffer,
                                                uint64_t bufferOffset,
                                                const void *data,
                                                size_t size)
  {
    return {};
  }

  bool GPUQueueImpl::hasPendingCommands() const
  {
    return has_pending_commands_;
  }

  gpu::MaybeError GPUQueueImpl::waitForIdleForDestructionImpl()
  {
    return {};
  }
}
