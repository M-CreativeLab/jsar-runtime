#pragma once

#include <vector>
#include <optional>
#include <string>

#include "./gpu_base.hpp"
#include "./gpu_bind_group.hpp"

namespace commandbuffers
{
  class GPUPipelineLayout : public GPUHandle
  {
  private:
    std::vector<GPUBindGroupLayout> bind_group_layouts_;
  };

  class GPURenderPipelineDescriptor
  {
  public:
    std::optional<std::string> label;
  };

  class GPURenderPipeline : public GPUHandle
  {
  public:
    GPUBindGroupLayout &getBindGroupLayout(size_t index)
    {
      if (index < bind_group_layouts_.size())
        return bind_group_layouts_[index];
      throw std::out_of_range("Bind group layout index out of range");
    }

  private:
    std::vector<GPUBindGroupLayout> bind_group_layouts_;
  };
}
