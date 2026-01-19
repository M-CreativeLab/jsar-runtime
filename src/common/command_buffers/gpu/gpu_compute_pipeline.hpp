#pragma once

#include <string_view>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>

namespace commandbuffers
{
  struct GPUComputePipelineDescriptor
  {
    std::string_view label;
    GPUPipelineLayoutBase *layout = nullptr;
    GPUComputeState compute;
  };

  class GPUComputePipelineBase : public GPUPipelineBase
  {
  };
}
