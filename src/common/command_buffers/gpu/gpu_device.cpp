#include <common/command_buffers/gpu/gpu_device.hpp>

namespace commandbuffers
{
  GPUDeviceBase::GPUDeviceBase(GPUAdapterBase *adapter,
                               GPUDeviceDescriptor &descriptor)
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

  Ref<GPUBindGroupLayoutBase> GPUDeviceBase::getOrCreateBindGroupLayout()
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
}
