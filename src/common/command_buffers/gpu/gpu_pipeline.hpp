#pragma once

#include <vector>
#include <optional>
#include <string>

#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group.hpp>

namespace commandbuffers
{
  class GPUPipelineLayoutBase : public GPUHandle
  {
  private:
    std::vector<GPUBindGroupLayoutBase> bind_group_layouts_;
  };

  class GPURenderPipelineBase;
  class GPUComputePipelineBase;

  class GPUPipelineBase : public GPUHandle
  {
  public:
    virtual const GPUComputePipelineBase *getAsComputePipeline() const;
    virtual GPUComputePipelineBase *getAsComputePipeline();

    virtual const GPURenderPipelineBase *getAsRenderPipeline() const;
    virtual GPURenderPipelineBase *getAsRenderPipeline();

    GPUPipelineBase *layout();
    const GPUPipelineLayoutBase *layout() const;

  protected:
    GPUPipelineBase(std::shared_ptr<GPUDeviceBase> device,
                    std::shared_ptr<GPUPipelineLayoutBase> layout,
                    std::string_view label);
    GPUPipelineBase(std::shared_ptr<GPUDeviceBase> device,
                    GPUHandle::ErrorTag,
                    std::string_view label);

  private:
    GPUShaderStage stage_mask_ = GPUShaderStage::kNone;
    std::shared_ptr<GPUPipelineLayoutBase> layout_;
  };
}
