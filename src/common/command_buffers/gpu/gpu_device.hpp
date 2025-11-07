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
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_render_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_compute_pipeline.hpp>

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

    Ref<GPUBindGroupLayoutBase> getOrCreateBindGroupLayout(
      const GPUBindGroupLayoutDescriptor &descriptor);

    GPUBindGroupLayoutBase *getEmptyBindGroupLayout();
    GPUPipelineLayoutBase *getEmptyPipelineLayout();

    // Object creation methods that be used in a reentrant manner.
    Ref<GPUBindGroupBase> createBindGroup(const GPUBindGroupDescriptor *descriptor,
                                          GPUUsageValidationMode mode = GPUUsageValidationMode::kDefault);
    Ref<GPUBindGroupLayoutBase> createBindGroupLayout(const GPUBindGroupLayoutDescriptor *descriptor,
                                                      bool allowInternalBinding = false);
    Ref<GPUBufferBase> createBuffer(const GPUBufferDescriptor *rawDescriptor);
    Ref<GPUCommandEncoder> createCommandEncoder(const GPUCommandEncoderDescriptor *descriptor = nullptr);
    Ref<GPUComputePipelineBase> createComputePipeline(const GPUComputePipelineDescriptor *descriptor);

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
    virtual Ref<GPUBindGroupBase> createBindGroupImpl(
      const GPUBindGroupDescriptor &descriptor) = 0;
    virtual Ref<GPUBindGroupLayoutInternalBase> createBindGroupLayoutImpl(
      const GPUBindGroupLayoutDescriptor &descriptor) = 0;

    Ref<GPUBindGroupLayoutBase> createEmptyBindGroupLayout();
    Ref<GPUPipelineLayoutBase> createEmptyPipelineLayout();

    Ref<GPUAdapterBase> adapter_;

    Ref<GPUBindGroupLayoutBase> empty_bind_group_layout_;
    Ref<GPUPipelineLayoutBase> empty_pipeline_layout_;
  };
}
