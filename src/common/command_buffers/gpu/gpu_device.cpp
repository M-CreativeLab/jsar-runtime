#include <common/command_buffers/gpu/gpu_device.hpp>

namespace commandbuffers
{
  GPUDeviceBase::GPUDeviceBase(Ref<GPUAdapterBase> adapter, const GPUDeviceDescriptor &descriptor)
      : adapter_(adapter)
  {
  }

  const GPUAdapterInfo &GPUDeviceBase::adapterInfo() const
  {
    return adapter_->info();
  }

  const GPUSupportedFeatures &GPUDeviceBase::features() const
  {
    return features_;
  }

  const GPUSupportedLimits &GPUDeviceBase::limits() const
  {
    return limits_;
  }

  bool GPUDeviceBase::validateHandle(const GPUHandle &handle) const
  {
    return false;
  }

  GPUInstance *GPUDeviceBase::getInstance() const
  {
    return adapter_->instance();
  }

  GPUAdapterBase *GPUDeviceBase::getAdapter() const
  {
    return adapter_.get();
  }

  gpu::PhysicalDeviceBase *GPUDeviceBase::getPhysicalDevice() const
  {
    return adapter_->physicalDevice();
  }

  Ref<GPUBindGroupLayoutBase> GPUDeviceBase::getOrCreateBindGroupLayout(
    const GPUBindGroupLayoutDescriptor &descriptor)
  {
    return nullptr;
  }

  GPUBindGroupLayoutBase *GPUDeviceBase::getEmptyBindGroupLayout()
  {
    assert(empty_bind_group_layout_ != nullptr);
    return empty_bind_group_layout_.get();
  }

  GPUPipelineLayoutBase *GPUDeviceBase::getEmptyPipelineLayout()
  {
    assert(empty_pipeline_layout_ != nullptr);
    return empty_pipeline_layout_.get();
  }

  // Object creation methods that be used in a reentrant manner.
  Ref<GPUBindGroupBase> GPUDeviceBase::createBindGroup(
    const GPUBindGroupDescriptor *descriptor,
    GPUUsageValidationMode mode)
  {
    return createBindGroupImpl(*descriptor);
  }

  Ref<GPUBindGroupLayoutBase> GPUDeviceBase::createBindGroupLayout(const GPUBindGroupLayoutDescriptor *descriptor,
                                                                   bool allowInternalBinding)
  {
    return nullptr;
  }

  Ref<GPUBufferBase> GPUDeviceBase::createBuffer(const GPUBufferDescriptor *rawDescriptor)
  {
    return nullptr;
  }

  Ref<GPUCommandEncoder> GPUDeviceBase::createCommandEncoder(const GPUCommandEncoderDescriptor *descriptor)
  {
    return nullptr;
  }

  Ref<GPUComputePipelineBase> GPUDeviceBase::createComputePipeline(const GPUComputePipelineDescriptor *descriptor)
  {
    return nullptr;
  }

  Ref<GPUShaderModuleBase> GPUDeviceBase::createShaderModule(const GPUShaderModuleDescriptor *descriptor,
                                                             const std::vector<wgsl::Extension> &internalExtensions)
  {
    return nullptr;
  }

  bool GPUDeviceBase::isValidationEnabled() const
  {
    // return !IsToggleEnabled(Toggle::SkipValidation);
    return false;
  }

  bool GPUDeviceBase::isRobustnessEnabled() const
  {
    // return !IsToggleEnabled(Toggle::DisableRobustness);
    return false;
  }

  bool GPUDeviceBase::isCompatibilityMode() const
  {
    // return !HasFeature(Feature::CoreFeaturesAndLimits);
    return false;
  }

  bool GPUDeviceBase::isImmediateErrorHandlingEnabled() const
  {
    // return mIsImmediateErrorHandlingEnabled;
    return false;
  }

  bool GPUDeviceBase::mayRequireDuplicationOfIndirectParameters() const
  {
    return false;
  }

  void GPUDeviceBase::setLabelImpl()
  {
  }

  bool GPUDeviceBase::reduceMemoryUsageImpl()
  {
    return false;
  }

  void GPUDeviceBase::performIdleTasksImpl()
  {
  }

  Ref<GPUBindGroupLayoutBase> GPUDeviceBase::createEmptyBindGroupLayout()
  {
    GPUBindGroupLayoutDescriptor desc = {};
    desc.entryCount = 0;
    desc.entries = nullptr;

    return getOrCreateBindGroupLayout(desc);
  }

  Ref<GPUPipelineLayoutBase> GPUDeviceBase::createEmptyPipelineLayout()
  {
    // TODO(yorkie): implement this.
    return nullptr;
  }
}
