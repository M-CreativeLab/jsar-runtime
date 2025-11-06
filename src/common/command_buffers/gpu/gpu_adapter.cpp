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
                                     function<void(unique_ptr<GPUDeviceBase>)> callback)
  {
    auto device = createDevice(descriptor);
    callback(move(device));
  }

  unique_ptr<GPUDeviceBase> GPUAdapterBase::createDevice(const GPUDeviceDescriptor *descriptor)
  {
    // Implementation for creating a GPU device based on the descriptor
    return nullptr;
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
}
