#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace commandbuffers
{
  namespace gpu
  {
    class BackendConnection;
    class PhysicalDeviceBase;
  }

  struct GPUInstanceDescriptor
  {
    size_t requiredFeatureCount = 0;
    GPUFeatureName const *requiredFeatures = nullptr;
    GPUSupportedLimits *requiredLimits = nullptr;
  };

  class GPUInstance final
  {
  public:
    static Ref<GPUInstance> Create(const GPUInstanceDescriptor *descriptor = nullptr);

    void registerBackend(gpu::BackendConnection *backend);
    void addDevice(Ref<GPUDeviceBase>);
    void removeDevice(Ref<GPUDeviceBase>);

    bool hasFeature(GPUFeatureName feature) const;

  private:
    explicit GPUInstance();
    virtual ~GPUInstance() = default;

    void initialize(const GPUInstanceDescriptor &descriptor);
    Ref<GPUAdapterBase> createAdapter(Ref<gpu::PhysicalDeviceBase> physicalDevice,
                                      GPUFeatureLevel featureLevel,
                                      GPUPowerPreference powerPreference);

    gpu::BackendConnection *getBackendConnection() const;

  private:
    std::unordered_set<GPUFeatureName> instance_features_;
    std::vector<Ref<GPUDeviceBase>> devices_list_;
    std::unique_ptr<gpu::BackendConnection> backend_;
  };
}
