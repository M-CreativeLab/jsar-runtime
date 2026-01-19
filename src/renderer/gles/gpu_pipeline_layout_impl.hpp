#pragma once

#include <common/command_buffers/gpu/gpu_pipeline_layout.hpp>
#include <renderer/gles/common.hpp>

namespace gles
{
  class GPUPipelineLayoutImpl : public commandbuffers::GPUPipelineLayoutBase
  {
  public:
    GPUPipelineLayoutImpl(Ref<commandbuffers::GPUDeviceBase> device,
                          const commandbuffers::GPUPipelineLayoutDescriptor &descriptor)
        : commandbuffers::GPUPipelineLayoutBase(device, descriptor)
    {
    }
  };
}
