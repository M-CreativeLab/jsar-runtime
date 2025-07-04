#pragma once

#include "./gpu_base.hpp"

namespace commandbuffers
{
  class GPUCommandBuffer : public GPUHandle
  {
  public:
    virtual ~GPUCommandBuffer() = default;
  };
}
