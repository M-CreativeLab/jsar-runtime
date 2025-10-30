#pragma once

#include <optional>
#include <vector>
#include <variant>

#include "./gpu_base.hpp"
#include "./gpu_bind_group_layout.hpp"

namespace commandbuffers
{
  class GPUBindGroupBase : public GPUHandle
  {
  public:
    virtual ~GPUBindGroupBase() = default;

  private:
    std::shared_ptr<GPUBindGroupLayoutBase> *layout_ = nullptr;
  };
}
