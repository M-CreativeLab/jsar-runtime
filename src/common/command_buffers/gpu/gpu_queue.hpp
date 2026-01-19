#pragma once

#include <string_view>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/execution_queue.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;
  struct GPUQueueDescriptor
  {
    std::string_view label;
  };

  class GPUQueueBase : public gpu::ExecutionQueueBase
  {
  public:
    static Ref<GPUQueueBase> MakeError(Ref<GPUDeviceBase> device, std::string_view label);

    GPUHandleType type() const override
    {
      return GPUHandleType::kQueue;
    }

    void submit(uint32_t commandCount, GPUCommandBufferBase *const *commands);
    void writeBuffer(GPUBufferBase *buffer,
                     uint64_t bufferOffset,
                     const void *data,
                     size_t size);

  protected:
    GPUQueueBase(Ref<GPUDeviceBase> device, const GPUQueueDescriptor &descriptor);
    GPUQueueBase(Ref<GPUDeviceBase> device, GPUHandle::ErrorTag, std::string_view label);

    virtual gpu::MaybeError submitImpl(uint32_t commandCount, GPUCommandBufferBase *const *commands) = 0;
    virtual gpu::MaybeError writeBufferImpl(GPUBufferBase *buffer,
                                            uint64_t bufferOffset,
                                            const void *data,
                                            size_t size);

  private:
    gpu::MaybeError validateSubmit(uint32_t commandCount, GPUCommandBufferBase *const *commands) const;
    gpu::MaybeError validateOnSubmittedWorkDone() const;

    gpu::MaybeError submitInternal(uint32_t commandCount, GPUCommandBufferBase *const *commands);
  };
}
