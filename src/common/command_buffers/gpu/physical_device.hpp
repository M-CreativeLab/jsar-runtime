#pragma once

#include <cstdint>
#include <string>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_info.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>

namespace commandbuffers::gpu
{
  class PhysicalDeviceBase
  {
  public:
    explicit PhysicalDeviceBase(GPUBackendType);

    void initialize();
    Ref<GPUDeviceBase> createDevice(Ref<GPUAdapterBase> adapter, const GPUDeviceDescriptor &descriptor);

    uint32_t vendorId() const;
    uint32_t deviceId() const;
    const std::string &vendorName() const;
    const std::string &architectureName() const;
    const std::string &name() const;
    const gpu_info::DriverVersion &driverVersion() const;
    const std::string &driverDescription() const;
    GPUAdapterType adapterType() const;
    GPUBackendType backendType() const;
    uint32_t subgroupMinSize() const;
    uint32_t subgroupMaxSize() const;

    virtual bool supportsExternalImages() const = 0;
    virtual bool supportsFeatureLevel(GPUFeatureLevel featureLevel, GPUInstance *instance) const = 0;

  protected:
    void enableFeature(GPUFeatureName feature);

  private:
    virtual Ref<GPUDeviceBase> createDeviceImpl(Ref<GPUAdapterBase> adapter,
                                                const GPUDeviceDescriptor &descriptor) = 0;
    virtual void initializeImpl() = 0;
    virtual void initializeSupportedFeaturesImpl() = 0;
    virtual void initializeVendorArchitectureImpl();

  protected:
    uint32_t vendor_id_ = 0xFFFFFFFF;
    uint32_t device_id_ = 0xFFFFFFFF;
    std::string vendor_name_ = "Unknown";
    std::string arch_name_ = "Unknown";
    std::string name_ = "";
    GPUAdapterType adapter_type_ = GPUAdapterType::kUnknown;
    gpu_info::DriverVersion driver_version_;
    std::string driver_description_ = "";

    uint32_t subgroup_min_size_ = 4u;
    uint32_t subgroup_max_size_ = 128u;

  private:
    GPUBackendType backend_type_ = GPUBackendType::kUndefined;
  };
}
