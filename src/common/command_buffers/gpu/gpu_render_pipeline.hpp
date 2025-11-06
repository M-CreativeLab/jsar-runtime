#pragma once

#include <optional>
#include <string_view>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;
  struct GPURenderPipelineDescriptor
  {
    std::string_view label;
    Ref<GPUPipelineLayoutBase> layout = nullptr;
    GPUVertexState vertex;
    GPUPrimitiveState primitive;
    GPUDepthStencilState const *depthStencil = nullptr;
    GPUMultisampleState multisample;
    GPUFragmentState const *fragment = nullptr;
  };

  class GPURenderPipelineBase : public GPUPipelineBase
  {
  public:
    GPURenderPipelineBase(Ref<GPUDeviceBase> device, const GPURenderPipelineDescriptor &descriptor);
  };
}
