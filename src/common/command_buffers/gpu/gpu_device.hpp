#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include <string_view>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_bind_group.hpp>

namespace commandbuffers
{
  namespace gpu
  {
    class PhysicalDeviceBase;
  }

  struct GPUDeviceDescriptor
  {
    std::string_view label;
    size_t requiredFeatureCount = 0;
    GPUFeatureName const *requiredFeatures = nullptr;
    GPUSupportedLimits *requiredLimits = nullptr;
  };

  class GPUDeviceBase
  {
  public:
    GPUDeviceBase(Ref<GPUAdapterBase> adapter, const GPUDeviceDescriptor &descriptor);
    virtual ~GPUDeviceBase() = default;

  public:
    const GPUAdapterInfo &adapterInfo() const;
    const GPUSupportedFeatures &features() const;
    const GPUSupportedLimits &limits() const;

    bool validateHandle(const GPUHandle &handle) const;

    GPUInstance *getInstance() const;
    GPUAdapterBase *getAdapter() const;
    gpu::PhysicalDeviceBase *getPhysicalDevice() const;

    virtual std::unique_ptr<GPUCommandBufferBase> createCommandBuffer(
      GPUCommandEncoder &encoder,
      const GPUCommandBufferDescriptor *descriptor = nullptr) = 0;

    Ref<GPUBindGroupLayoutBase> getOrCreateBindGroupLayout();

    bool isValidationEnabled() const;
    bool isRobustnessEnabled() const;
    bool isCompatibilityMode() const;
    bool isImmediateErrorHandlingEnabled() const;

    virtual bool mayRequireDuplicationOfIndirectParameters() const;

  protected:
    GPUAdapterInfo adapter_info_;
    GPUSupportedFeatures features_;
    GPUSupportedLimits limits_;

  private:
    Ref<GPUAdapterBase> adapter_;
  };
}
