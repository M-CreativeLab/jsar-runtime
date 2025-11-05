#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/gpu_physical_device.hpp>

using namespace std;

namespace commandbuffers
{
  bool GPUSupportedLimits::operator==(const GPUSupportedLimits &rhs) const
  {
    const auto &lhs = std::tie(maxTextureDimension1D,
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
    return lhs == std::tie(rhs.maxTextureDimension1D,
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

  GPUAdapterBase::GPUAdapterBase(shared_ptr<GPUInstance> instance,
                                 shared_ptr<GPUPhysicalDeviceBase> physicalDevice,
                                 GPUFeatureLevel level,
                                 GPUPowerPreference powerPreference)
      : ErrorMonad()
      , instance_(instance)
      , physical_device_(physicalDevice)
      , feature_level_(level)
      , power_preference_(powerPreference)
  {
  }
}
