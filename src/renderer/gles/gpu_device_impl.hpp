#pragma once

#include <string>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>

namespace gles
{
  class GPUDeviceImpl final : public commandbuffers::GPUDeviceBase
  {
  public:
    static Ref<GPUDeviceImpl> Create(Ref<commandbuffers::GPUAdapterBase> adapter,
                                     const commandbuffers::GPUDeviceDescriptor &descriptor);

  private:
    GPUDeviceImpl(Ref<commandbuffers::GPUAdapterBase> adapter,
                  const commandbuffers::GPUDeviceDescriptor &descriptor);

    bool initialize(const commandbuffers::GPUDeviceDescriptor &descriptor);

  public:
    std::unique_ptr<commandbuffers::GPUCommandBufferBase> createCommandBuffer(
      commandbuffers::GPUCommandEncoder &encoder,
      const commandbuffers::GPUCommandBufferDescriptor *descriptor = nullptr) override;

  private:
    Ref<commandbuffers::GPUBindGroupBase> createBindGroupImpl(
      const commandbuffers::GPUBindGroupDescriptor &) override;
    Ref<commandbuffers::GPUBindGroupLayoutInternalBase> createBindGroupLayoutImpl(
      const commandbuffers::GPUBindGroupLayoutDescriptor &) override;
    Ref<commandbuffers::GPUBufferBase> createBufferImpl(
      const commandbuffers::GPUBufferDescriptor &) override;
    Ref<commandbuffers::GPUPipelineLayoutBase> createPipelineLayoutImpl(
      const commandbuffers::GPUPipelineLayoutDescriptor &) override;
    Ref<commandbuffers::GPUShaderModuleBase> createShaderModuleImpl(
      const commandbuffers::GPUShaderModuleDescriptor &,
      const std::vector<wgsl::Extension> &) override;
    Ref<commandbuffers::GPUTextureBase> createTextureImpl(
      const commandbuffers::GPUTextureDescriptor &) override;
    Ref<commandbuffers::GPUTextureViewBase> createTextureViewImpl(
      Ref<commandbuffers::GPUTextureBase> texture,
      const commandbuffers::GPUTextureViewDescriptor &) override;
    Ref<commandbuffers::GPUComputePipelineBase> createUninitializedComputePipelineImpl(
      const commandbuffers::GPUComputePipelineDescriptor &) override;
    Ref<commandbuffers::GPURenderPipelineBase> createUninitializedRenderPipelineImpl(
      const commandbuffers::GPURenderPipelineDescriptor &) override;

    bool tickImpl() override;
  };
}
