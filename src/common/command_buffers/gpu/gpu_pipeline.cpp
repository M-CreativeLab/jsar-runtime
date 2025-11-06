#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_render_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_compute_pipeline.hpp>

using namespace std;

namespace commandbuffers
{
  GPUPipelineBase::GPUPipelineBase(Ref<GPUDeviceBase> device,
                                   Ref<GPUPipelineLayoutBase> layout,
                                   string_view label)
      : GPUHandle(device, label)
  {
  }

  GPUPipelineBase::GPUPipelineBase(shared_ptr<GPUDeviceBase> device,
                                   GPUHandle::ErrorTag,
                                   string_view label)
      : GPUHandle(device, ErrorTag{}, label)
  {
  }

  const GPUComputePipelineBase *GPUPipelineBase::getAsComputePipeline() const
  {
    return nullptr;
  }

  GPUComputePipelineBase *GPUPipelineBase::getAsComputePipeline()
  {
    return nullptr;
  }

  const GPURenderPipelineBase *GPUPipelineBase::getAsRenderPipeline() const
  {
    return nullptr;
  }

  GPURenderPipelineBase *GPUPipelineBase::getAsRenderPipeline()
  {
    return nullptr;
  }

  GPUPipelineLayoutBase *GPUPipelineBase::layout()
  {
    return layout_.get();
  }

  const GPUPipelineLayoutBase *GPUPipelineBase::layout() const
  {
    return layout_.get();
  }
}
