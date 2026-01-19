#include <common/command_buffers/gpu/gpu_queue.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace commandbuffers
{
  class GPUErrorQueue : public GPUQueueBase
  {
  public:
    explicit GPUErrorQueue(Ref<GPUDeviceBase> device, std::string_view label)
        : GPUQueueBase(device, GPUHandle::kError, label)
    {
    }

  private:
    gpu::MaybeError submitImpl(uint32_t commandCount, GPUCommandBufferBase *const *commands) override
    {
      assert(false);
    }
    bool hasPendingCommands() const override
    {
      assert(false);
    }
    gpu::MaybeError waitForIdleForDestructionImpl() override
    {
      return {};
    }
  };

  GPUQueueBase::GPUQueueBase(Ref<GPUDeviceBase> device, const GPUQueueDescriptor &descriptor)
      : gpu::ExecutionQueueBase(device, descriptor.label)
  {
  }

  GPUQueueBase::GPUQueueBase(Ref<GPUDeviceBase> device, GPUHandle::ErrorTag, std::string_view label)
      : gpu::ExecutionQueueBase(device, label)
  {
  }

  // static
  Ref<GPUQueueBase> GPUQueueBase::MakeError(Ref<GPUDeviceBase> device, std::string_view label)
  {
    return AcquireRef(new GPUErrorQueue(device, label));
  }

  void GPUQueueBase::submit(uint32_t commandCount, GPUCommandBufferBase *const *commands)
  {
    assert(submitInternal(commandCount, commands).IsSuccess());
  }

  void GPUQueueBase::writeBuffer(GPUBufferBase *buffer,
                                 uint64_t bufferOffset,
                                 const void *data,
                                 size_t size)
  {
  }

  gpu::MaybeError GPUQueueBase::writeBufferImpl(GPUBufferBase *buffer,
                                                uint64_t bufferOffset,
                                                const void *data,
                                                size_t size)
  {
    return buffer->uploadData(bufferOffset, data, size);
  }

  gpu::MaybeError GPUQueueBase::validateSubmit(uint32_t commandCount, GPUCommandBufferBase *const *commands) const
  {
    return {};
  }

  gpu::MaybeError GPUQueueBase::validateOnSubmittedWorkDone() const
  {
    return {};
  }

  gpu::MaybeError GPUQueueBase::submitInternal(uint32_t commandCount, GPUCommandBufferBase *const *commands)
  {
    assert(device()->validateIsAlive().IsSuccess());

    if (device()->isValidationEnabled())
    {
      assert(validateSubmit(commandCount, commands).IsSuccess());
    }
    assert(!isError());

    inSubmit = true;
    {
      if (submitImpl(commandCount, commands).IsError())
      {
        assert(false && "submitImpl failed");
      }
    }
    inSubmit = false;

    // Call tick() to flush pending work.
    assert(device()->tick().IsSuccess());
    return {};
  }
}
