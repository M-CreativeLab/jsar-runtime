#pragma once

#include <vector>
#include <optional>
#include <string>

#include <common/utility.hpp>
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

    GPUPipelineLayoutBase *layout();
    const GPUPipelineLayoutBase *layout() const;

  protected:
    GPUPipelineBase(Ref<GPUDeviceBase> device, Ref<GPUPipelineLayoutBase> layout, std::string_view label);
    GPUPipelineBase(Ref<GPUDeviceBase> device, GPUHandle::ErrorTag, std::string_view label);

  private:
    GPUShaderStage stage_mask_ = GPUShaderStage::kNone;
    Ref<GPUPipelineLayoutBase> layout_;
  };
}
