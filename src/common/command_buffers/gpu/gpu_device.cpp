#include <common/command_buffers/gpu/gpu_device.hpp>

namespace commandbuffers
{
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
}
