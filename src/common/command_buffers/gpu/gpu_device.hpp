#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "./gpu_base.hpp"
#include "./gpu_command_buffer.hpp"

namespace commandbuffers
{
  class GPUQueue : public GPUHandle
  {
  public:
    virtual ~GPUQueue() = default;

  public:
    virtual void submit(const std::vector<GPUCommandBuffer> &) = 0;
  };

  class GPUDevice : public GPUHandle
  {
  public:
    virtual ~GPUDevice() = default;

  public:
    GPUQueue &queueRef()
    {
      assert(queue_ != nullptr);
      return *queue_;
    }

  private:
    std::unique_ptr<GPUQueue> queue_;
  };
}
