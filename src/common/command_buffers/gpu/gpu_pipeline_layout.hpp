#pragma once

#include <vector>
#include <optional>
#include <string>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout.hpp>

namespace commandbuffers
{
  class GPUPipelineLayoutBase : public GPUHandle
  {
  private:
    std::vector<GPUBindGroupLayoutBase> bind_group_layouts_;
  };
}
