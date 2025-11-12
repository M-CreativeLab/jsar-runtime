#pragma once

#include <vector>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/wgsl/enums.h>

namespace commandbuffers
{
  class GPUDeviceBase;

  struct GPUShaderModuleDescriptor
  {
    std::string_view label;
    std::string_view code;
    std::vector<std::string> hints;
  };

  class GPUShaderModuleBase : public GPUHandle
  {
  public:
    GPUShaderModuleBase(Ref<GPUDeviceBase> device,
                        const GPUShaderModuleDescriptor &descriptor,
                        std::vector<wgsl::Extension> internalExtensions);

    GPUHandleType type() const override
    {
      return GPUHandleType::kShaderModule;
    }
  };
}
