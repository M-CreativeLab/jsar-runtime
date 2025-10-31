#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace commandbuffers
{
  struct GPUInstanceDescriptor
  {
    size_t requiredFeatureCount = 0;
    GPUFeatureName const *requiredFeatures = nullptr;
    GPUSupportedLimits *requiredLimits = nullptr;
  };

  class GPUInstanceBase
  {
  public:
    static std::unique_ptr<GPUInstanceBase> Create(const GPUInstanceDescriptor *descriptor = nullptr);

    void addDevice(std::shared_ptr<GPUDeviceBase>);
    void removeDevice(std::shared_ptr<GPUDeviceBase>);

    bool hasFeature(GPUFeatureName feature) const;

  private:
    explicit GPUInstanceBase();
    virtual ~GPUInstanceBase() = default;

    void initialize(const GPUInstanceDescriptor &descriptor);
    std::shared_ptr<GPUAdapterBase> createAdapter();

  private:
    std::unordered_set<GPUFeatureName> instance_features_;
    std::vector<std::shared_ptr<GPUDeviceBase>> devices_list_;
  };
}
