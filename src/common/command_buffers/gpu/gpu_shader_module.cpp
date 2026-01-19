#include <common/command_buffers/gpu/gpu_shader_module.hpp>
#include "utility.hpp"

using namespace std;

namespace commandbuffers
{
  GPUShaderModuleBase::GPUShaderModuleBase(Ref<GPUDeviceBase> device,
                                           const GPUShaderModuleDescriptor &descriptor,
                                           vector<wgsl::Extension> internal_extensions)
      : GPUHandle(device, descriptor.label)
      , type_(Type::kGLSL)
      , original_spirv_()
      , wgsl_code_(descriptor.code)
      , internal_extensions_(move(internal_extensions))
  {
  }

  GPUShaderModuleBase::GPUShaderModuleBase(Ref<GPUDeviceBase> device,
                                           GPUHandle::ErrorTag tag,
                                           string_view label,
                                           gpu::ParsedCompilationMessages &&compilation_messages)
      : GPUHandle(device, tag, label)
      , type_(Type::kUndefined)
  {
  }

  // static
  Ref<GPUShaderModuleBase> GPUShaderModuleBase::MakeError(Ref<GPUDeviceBase> device,
                                                          string_view label,
                                                          gpu::ParsedCompilationMessages &&compilation_messages)
  {
    return AcquireRef(new GPUShaderModuleBase(
      device,
      GPUHandle::kError,
      label,
      move(compilation_messages)));
  }

  void GPUShaderModuleBase::initialize()
  {
  }

  std::unique_ptr<gpu::ErrorData> GPUShaderModuleBase::getInitializationError()
  {
    return nullptr;
  }

  bool GPUShaderModuleBase::hasEntryPoint(std::string_view entry_point) const
  {
    return false;
  }
}
