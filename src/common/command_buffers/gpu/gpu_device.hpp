#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "./gpu_base.hpp"
#include "./gpu_adapter.hpp"
#include "./gpu_command_buffer.hpp"
#include "./gpu_command_encoder.hpp"

namespace commandbuffers
{
  class GPUQueue : public GPUHandle
  {
  public:
    virtual ~GPUQueue() = default;

  public:
    virtual void submit(const std::vector<std::shared_ptr<GPUCommandBuffer>> &) = 0;
  };

  /**
   * The `GPUDevice` interface represents a logical GPU device.
   * 
   * In usually, at client-side WebGPU implementation, it implements the `GPUDevice` interface to send 
   * `GPUCommandBuffer` to the renderer. And at server-side's renderer, it implements the same `GPUDevice` interface to
   * execute the `GPUCommandBuffer` on the corresponding graphics API (e.g. OpenGL, Vulkan, etc.) via RHI.
   */
  class GPUDevice : public GPUHandle
  {
  public:
    virtual ~GPUDevice() = default;

  public:
    const GPUAdapterInfo &adapterInfo() const
    {
      return adapter_info_;
    }
    const GPUSupportedFeatures &features() const
    {
      return features_;
    }
    const GPUSupportedLimits &limits() const
    {
      return limits_;
    }
    GPUQueue &queueRef()
    {
      assert(queue_ != nullptr);
      return *queue_;
    }

    virtual std::unique_ptr<GPUCommandEncoder> createCommandEncoder(std::optional<std::string> label) = 0;

  protected:
    GPUAdapterInfo adapter_info_;
    GPUSupportedFeatures features_;
    GPUSupportedLimits limits_;
    std::unique_ptr<GPUQueue> queue_;
  };
}
