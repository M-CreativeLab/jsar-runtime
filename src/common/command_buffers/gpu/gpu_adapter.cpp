#include <common/command_buffers/gpu/backend_connection.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/physical_device.hpp>

using namespace std;

namespace commandbuffers
{
  bool GPUSupportedLimits::operator==(const GPUSupportedLimits &rhs) const
  {
    const auto &lhs = tie(maxTextureDimension1D,
                          maxTextureDimension2D,
                          maxTextureDimension3D,
                          maxTextureArrayLayers,
                          maxBindGroups,
                          maxBindingsPerBindGroup,
                          maxDynamicUniformBuffersPerPipelineLayout,
                          maxDynamicStorageBuffersPerPipelineLayout,
                          maxSampledTexturesPerShaderStage,
                          maxSamplersPerShaderStage,
                          maxStorageBuffersPerShaderStage,
                          maxStorageTexturesPerShaderStage,
                          maxUniformBuffersPerShaderStage,
                          maxUniformBufferBindingSize,
                          maxStorageBufferBindingSize,
                          minUniformBufferOffsetAlignment,
                          minStorageBufferOffsetAlignment,
                          maxVertexBuffers,
                          maxBufferSize,
                          maxVertexAttributes,
                          maxVertexBufferArrayStride,
                          maxInterStageShaderVariables,
                          maxColorAttachments,
                          maxColorAttachmentBytesPerSample,
                          maxComputeWorkgroupStorageSize,
                          maxComputeInvocationsPerWorkgroup,
                          maxComputeWorkgroupSizeX,
                          maxComputeWorkgroupSizeY,
                          maxComputeWorkgroupSizeZ,
                          maxComputeWorkgroupsPerDimension);
    return lhs == tie(rhs.maxTextureDimension1D,
                      rhs.maxTextureDimension2D,
                      rhs.maxTextureDimension3D,
                      rhs.maxTextureArrayLayers,
                      rhs.maxBindGroups,
                      rhs.maxBindingsPerBindGroup,
                      rhs.maxDynamicUniformBuffersPerPipelineLayout,
                      rhs.maxDynamicStorageBuffersPerPipelineLayout,
                      rhs.maxSampledTexturesPerShaderStage,
                      rhs.maxSamplersPerShaderStage,
                      rhs.maxStorageBuffersPerShaderStage,
                      rhs.maxStorageTexturesPerShaderStage,
                      rhs.maxUniformBuffersPerShaderStage,
                      rhs.maxUniformBufferBindingSize,
                      rhs.maxStorageBufferBindingSize,
                      rhs.minUniformBufferOffsetAlignment,
                      rhs.minStorageBufferOffsetAlignment,
                      rhs.maxVertexBuffers,
                      rhs.maxBufferSize,
                      rhs.maxVertexAttributes,
                      rhs.maxVertexBufferArrayStride,
                      rhs.maxInterStageShaderVariables,
                      rhs.maxColorAttachments,
                      rhs.maxColorAttachmentBytesPerSample,
                      rhs.maxComputeWorkgroupStorageSize,
                      rhs.maxComputeInvocationsPerWorkgroup,
                      rhs.maxComputeWorkgroupSizeX,
                      rhs.maxComputeWorkgroupSizeY,
                      rhs.maxComputeWorkgroupSizeZ,
                      rhs.maxComputeWorkgroupsPerDimension);
  }

  GPUAdapterBase::GPUAdapterBase(Ref<GPUInstance> instance,
                                 Ref<gpu::PhysicalDeviceBase> physicalDevice,
                                 GPUFeatureLevel level,
                                 GPUPowerPreference powerPreference)
      : ErrorMonad()
      , instance_(instance)
      , physical_device_(physicalDevice)
      , feature_level_(level)
      , power_preference_(powerPreference)
  {
  }

  GPUInstance *GPUAdapterBase::instance() const
  {
    return instance_.get();
  }

  const GPUAdapterInfo &GPUAdapterBase::info() const
  {
    return info_;
  }

  bool GPUAdapterBase::hasFeature(GPUFeatureName feature) const
  {
    return false;
  }

  void GPUAdapterBase::requestDevice(const GPUDeviceDescriptor *descriptor,
                                     function<void(GPUDeviceBase &)> callback)
  {
    auto device = createDevice(descriptor);
    callback(*device);
  }

  Ref<GPUDeviceBase> GPUAdapterBase::createDevice(const GPUDeviceDescriptor *descriptor)
  {
    GPUDeviceDescriptor default_descriptor{};
    const GPUDeviceDescriptor &desc = descriptor ? *descriptor : default_descriptor;
    return physical_device_->createDevice(shared_from_this(), desc);
  }

  gpu::PhysicalDeviceBase *GPUAdapterBase::physicalDevice()
  {
    return physical_device_.get();
  }

  const gpu::PhysicalDeviceBase *GPUAdapterBase::physicalDevice() const
  {
    return physical_device_.get();
  }

  GPUFeatureLevel GPUAdapterBase::featureLevel() const
  {
    return feature_level_;
  }

  const string &GPUAdapterBase::name() const
  {
    return physical_device_->name();
  }

  vector<Ref<GPUAdapterBase>> SortAdapters(vector<Ref<GPUAdapterBase>> adapters,
                                           const RequestAdapterOptions &options)
  {
    const bool highPerformance = options.powerPreference == GPUPowerPreference::kHighPerformance;
    const auto ComputeAdapterTypeRank = [&](const Ref<GPUAdapterBase> &a)
    {
      switch (a->physicalDevice()->adapterType())
      {
      case GPUAdapterType::kDiscreteGPU:
        return highPerformance ? 0 : 1;
      case GPUAdapterType::kIntegratedGPU:
        return highPerformance ? 1 : 0;
      case GPUAdapterType::kCPU:
        return 2;
      case GPUAdapterType::kUnknown:
        return 3;
      }
      assert(false && "Unhandled GPUAdapterType");
    };
    const auto ComputeBackendTypeRank = [](const Ref<GPUAdapterBase> &a)
    {
      switch (a->physicalDevice()->backendType())
      {
      // Sort backends generally in order of Core -> Compat -> Testing,
      // while preferring OS-specific backends like Metal/D3D.
      case GPUBackendType::kMetal:
      case GPUBackendType::kD3D12:
        return 0;
      case GPUBackendType::kVulkan:
        return 1;
      case GPUBackendType::kD3D11:
        return 2;
      case GPUBackendType::kOpenGLES:
        return 3;
      case GPUBackendType::kOpenGL:
        return 4;
      case GPUBackendType::kWebGPU:
        return 5;
      case GPUBackendType::kNull:
        return 6;
      case GPUBackendType::kUndefined:
        break;
      }
      assert(false && "Unhandled GPUBackendType");
    };

    sort(adapters.begin(), adapters.end(), [&](const Ref<GPUAdapterBase> &a, const Ref<GPUAdapterBase> &b) -> bool
         { return tuple(ComputeAdapterTypeRank(a), ComputeBackendTypeRank(a)) <
                  tuple(ComputeAdapterTypeRank(b), ComputeBackendTypeRank(b)); });
    return adapters;
  }
}
