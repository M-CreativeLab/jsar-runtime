#pragma once

#include <optional>
#include <vector>
#include <variant>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout.hpp>

namespace commandbuffers
{
  class GPUBindGroupBase : public GPUHandle
  {
  public:
    virtual ~GPUBindGroupBase() = default;

  private:
    Ref<GPUBindGroupLayoutBase> *layout_ = nullptr;
  };
}
