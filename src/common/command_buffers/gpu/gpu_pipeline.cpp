#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_render_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_compute_pipeline.hpp>

namespace commandbuffers
{
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
}
