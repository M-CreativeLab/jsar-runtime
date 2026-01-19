#pragma once

#include <vector>
#include <optional>
#include <string>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout.hpp>

namespace commandbuffers
{
  struct GPUPipelineLayoutDescriptor
  {
  };

  class GPUPipelineLayoutBase : public GPUHandle
  {
  public:
    GPUPipelineLayoutBase(Ref<GPUDeviceBase> device, const GPUPipelineLayoutDescriptor &descriptor)
        : GPUHandle(device, "GPUPipelineLayout")
    {
    }

    GPUHandleType type() const override final
    {
      return GPUHandleType::kPipelineLayout;
    }

  private:
    std::vector<GPUBindGroupLayoutBase> bind_group_layouts_;
  };
}
