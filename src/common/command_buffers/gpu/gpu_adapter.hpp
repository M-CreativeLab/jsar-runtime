#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  class GPUInstanceBase;
  class GPUPhysicalDeviceBase;
  struct GPUDeviceDescriptor;

  class GPUAdapterInfo
  {
  public:
    std::string architecture = "";
    std::string description = "";
    std::string device = "";
    std::string vendor = "";
    uint32_t subgroupMaxSize;
    uint32_t subgroupMinSize;

  public:
    friend std::ostream &operator<<(std::ostream &os, const GPUAdapterInfo &info)
    {
      os << "GPUAdapterInfo {" << std::endl
         << "  architecture: " << info.architecture << std::endl
         << "  description: " << info.description << std::endl
         << "  device: " << info.device << std::endl
         << "  vendor: " << info.vendor << std::endl
         << "  subgroupMaxSize: " << info.subgroupMaxSize << std::endl
         << "  subgroupMinSize: " << info.subgroupMinSize << std::endl
         << "}";
      return os;
    }
    std::string toString() const
    {
      std::stringstream ss;
      ss << *this;
      return ss.str();
    }
  };

  class GPUAdapterBase : ErrorMonad
  {
  public:
    GPUAdapterBase(std::shared_ptr<GPUInstanceBase> instance,
                   std::shared_ptr<GPUPhysicalDeviceBase> physicalDevice,
                   GPUFeatureLevel level,
                   GPUPowerPreference powerPreference);

    GPUInstanceBase *instance() const;
    const GPUAdapterInfo &info() const;
    bool hasFeature(GPUFeatureName) const;
    void requestDevice(const GPUDeviceDescriptor *descriptor,
                       std::function<void(std::unique_ptr<GPUDeviceBase>)> callback);
    std::unique_ptr<GPUDeviceBase> createDevice(const GPUDeviceDescriptor *descriptor = nullptr);

    GPUPhysicalDeviceBase *physicalDevice();
    const GPUPhysicalDeviceBase *physicalDevice() const;
    GPUFeatureLevel featureLevel() const;

    const std::string &name() const;

  private:
    std::shared_ptr<GPUInstanceBase> instance_;
    std::shared_ptr<GPUPhysicalDeviceBase> physical_device_;
    GPUFeatureLevel feature_level_;
    GPUPowerPreference power_preference_;

    bool use_tiered_limits = false;

    // The adapter becomes "consumed" once it has successfully been used to
    // create a device.
    bool adapter_is_consumed = false;
  };
}
