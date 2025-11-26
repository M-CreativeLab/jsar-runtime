#pragma once

#include <vector>
#include <unordered_map>

#include <common/utility.hpp>
#include <common/command_buffers/base.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_texture.hpp>

namespace renderer
{
  class TrRenderResource final
  {
  public:
    TrRenderResource(Ref<commandbuffers::GPUDeviceBase> device);

    Ref<commandbuffers::GPUPipelineBase> getPipeline(int id);
    Ref<commandbuffers::GPUBufferBase> getBuffer(int id);
    Ref<commandbuffers::GPUTextureBase> getTexture(int id);

  private:
    Ref<commandbuffers::GPUDeviceBase> device_;
    std::unordered_map<int, Ref<commandbuffers::GPUPipelineBase>> pipelines_;
    std::unordered_map<int, Ref<commandbuffers::GPUBufferBase>> buffers_;
    std::unordered_map<int, Ref<commandbuffers::GPUTextureBase>> textures_;
  };
}
