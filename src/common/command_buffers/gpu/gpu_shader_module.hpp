#pragma once

#include <vector>
#include <string>
#include <string_view>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/error_data.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/compilation_messages.hpp>
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
    static Ref<GPUShaderModuleBase> MakeError(Ref<GPUDeviceBase> device,
                                              std::string_view label,
                                              gpu::ParsedCompilationMessages &&compilation_messages);

    GPUShaderModuleBase(Ref<GPUDeviceBase> device,
                        const GPUShaderModuleDescriptor &descriptor,
                        std::vector<wgsl::Extension> internal_extensions);

    GPUHandleType type() const override
    {
      return GPUHandleType::kShaderModule;
    }

    void initialize();
    std::unique_ptr<gpu::ErrorData> getInitializationError();

    bool hasEntryPoint(std::string_view entry_point) const;

  private:
    GPUShaderModuleBase(Ref<GPUDeviceBase> device,
                        GPUHandle::ErrorTag tag,
                        std::string_view label,
                        gpu::ParsedCompilationMessages &&compilation_messages);

    enum class Type : uint8_t
    {
      kUndefined,
      kGLSL,
      kSpirv,
      kWGSL,
    };
    Type type_;
    std::vector<uint32_t> original_spirv_;
    std::string wgsl_code_;

    const std::vector<wgsl::Extension> internal_extensions_;
  };
}
