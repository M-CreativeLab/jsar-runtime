#include <common/command_buffers/gpu/physical_device.hpp>

using namespace std;

namespace commandbuffers::gpu
{
  PhysicalDeviceBase::PhysicalDeviceBase(GPUBackendType type)
      : backend_type_(type)
  {
  }

  void PhysicalDeviceBase::initialize()
  {
    initializeImpl();
    initializeVendorArchitectureImpl();
  }

  Ref<GPUDeviceBase> PhysicalDeviceBase::createDevice(Ref<GPUAdapterBase> adapter,
                                                      const GPUDeviceDescriptor &descriptor)
  {
    return createDeviceImpl(adapter, descriptor);
  }

  uint32_t PhysicalDeviceBase::vendorId() const
  {
    return vendor_id_;
  }

  uint32_t PhysicalDeviceBase::deviceId() const
  {
    return device_id_;
  }

  const string &PhysicalDeviceBase::vendorName() const
  {
    return vendor_name_;
  }

  const string &PhysicalDeviceBase::architectureName() const
  {
    return arch_name_;
  }

  const string &PhysicalDeviceBase::name() const
  {
    return name_;
  }

  const gpu_info::DriverVersion &PhysicalDeviceBase::driverVersion() const
  {
    return driver_version_;
  }

  const string &PhysicalDeviceBase::driverDescription() const
  {
    return driver_description_;
  }

  GPUAdapterType PhysicalDeviceBase::adapterType() const
  {
    return adapter_type_;
  }

  GPUBackendType PhysicalDeviceBase::backendType() const
  {
    return backend_type_;
  }

  uint32_t PhysicalDeviceBase::subgroupMinSize() const
  {
    return subgroup_min_size_;
  }

  uint32_t PhysicalDeviceBase::subgroupMaxSize() const
  {
    return subgroup_max_size_;
  }

  void PhysicalDeviceBase::enableFeature(GPUFeatureName feature)
  {
  }

  void PhysicalDeviceBase::initializeVendorArchitectureImpl()
  {
    vendor_name_ = gpu_info::GetVendorName(vendor_id_);
    arch_name_ = gpu_info::GetArchitectureName(vendor_id_, device_id_);
  }
}
