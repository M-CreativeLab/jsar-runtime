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

  class GPUInstance final : public std::enable_shared_from_this<GPUInstance>
  {
  public:
    static Ref<GPUInstance> Create(const GPUInstanceDescriptor *descriptor = nullptr);

    // Discovers and returns a single adapter based on the `options`.
    Ref<GPUAdapterBase> requestAdapter(const RequestAdapterOptions &options);

    // Discovers and returns a vector of adapters.
    // All systems adapters that can be found are returned if no options are passed.
    // Otherwise, returns adapters based on the `options`.
    std::vector<Ref<GPUAdapterBase>> enumerateAdapters(const RequestAdapterOptions *options = nullptr);

    void registerBackend(gpu::BackendConnection *backend);
    void addDevice(Ref<GPUDeviceBase>);
    void removeDevice(Ref<GPUDeviceBase>);

    bool hasFeature(GPUFeatureName feature) const;

    // Allow AcquireRef to access the private constructor.
    friend Ref<GPUInstance>(::AcquireRef)(GPUInstance *ptr);

  private:
    explicit GPUInstance();
    virtual ~GPUInstance() = default;

    void initialize(const GPUInstanceDescriptor &descriptor);
    Ref<GPUAdapterBase> createAdapter(Ref<gpu::PhysicalDeviceBase> physicalDevice,
                                      GPUFeatureLevel featureLevel,
                                      GPUPowerPreference powerPreference);

    gpu::BackendConnection *getBackendConnection() const;

    // Enumerate physical devices according to options and return them.
    std::vector<Ref<gpu::PhysicalDeviceBase>> enumeratePhysicalDevices(const RequestAdapterOptions &options);

  private:
    std::unordered_set<GPUFeatureName> instance_features_;
    std::vector<Ref<GPUDeviceBase>> devices_list_;
    std::unique_ptr<gpu::BackendConnection> backend_;
  };
}
