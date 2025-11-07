#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/backend_connection.hpp>
#include <common/command_buffers/gpu/physical_device.hpp>

using namespace std;

namespace commandbuffers
{
  // static
  Ref<GPUInstance> GPUInstance::Create(const GPUInstanceDescriptor *descriptor)
  {
    static constexpr GPUInstanceDescriptor kDefaultDesc = {};
    if (descriptor == nullptr)
    {
      descriptor = &kDefaultDesc;
    }

    auto instance = Ref<GPUInstance>(new GPUInstance(), [](GPUInstance *ptr)
                                     { delete ptr; });
    instance->initialize(*descriptor);
    return instance;
  }

  GPUInstance::GPUInstance()
  {
  }

  Ref<GPUAdapterBase> GPUInstance::requestAdapter(const RequestAdapterOptions &options)
  {
    auto adapters = enumerateAdapters(&options);
    if (adapters.empty())
    {
      return nullptr;
    }
    return adapters[0];
  }

  vector<Ref<GPUAdapterBase>> GPUInstance::enumerateAdapters(const RequestAdapterOptions *options)
  {
    static constexpr RequestAdapterOptions kDefaultOptions = {};
    if (options == nullptr)
    {
      options = &kDefaultOptions;
    }

    vector<Ref<GPUAdapterBase>> adapters;
    for (const auto &physicalDevice : enumeratePhysicalDevices(*options))
    {
      assert(physicalDevice->supportsFeatureLevel(options->featureLevel, this));
      adapters.push_back(createAdapter(physicalDevice, options->featureLevel, options->powerPreference));
    }
    return SortAdapters(std::move(adapters), *options);
  }

  void GPUInstance::registerBackend(gpu::BackendConnection *backend)
  {
    assert(backend != nullptr && "Backend connection cannot be null.");
    backend_ = unique_ptr<gpu::BackendConnection>(backend);
  }

  void GPUInstance::addDevice(Ref<GPUDeviceBase>)
  {
  }

  void GPUInstance::removeDevice(Ref<GPUDeviceBase>)
  {
  }

  bool GPUInstance::hasFeature(GPUFeatureName feature) const
  {
    return instance_features_.find(feature) != instance_features_.end();
  }

  void GPUInstance::initialize(const GPUInstanceDescriptor &descriptor)
  {
  }

  Ref<GPUAdapterBase> GPUInstance::createAdapter(Ref<gpu::PhysicalDeviceBase> physicalDevice,
                                                 GPUFeatureLevel featureLevel,
                                                 GPUPowerPreference powerPreference)
  {
    return AcquireRef(new GPUAdapterBase(shared_from_this(),
                                         physicalDevice,
                                         featureLevel,
                                         powerPreference));
  }

  gpu::BackendConnection *GPUInstance::getBackendConnection() const
  {
    return backend_.get();
  }

  vector<Ref<gpu::PhysicalDeviceBase>> GPUInstance::enumeratePhysicalDevices(const RequestAdapterOptions &options)
  {
    vector<Ref<gpu::PhysicalDeviceBase>> discoveredPhysicalDevices;
    if (backend_)
    {
      discoveredPhysicalDevices = backend_->discoverPhysicalDevices(options);
    }
    return discoveredPhysicalDevices;
  }
}
