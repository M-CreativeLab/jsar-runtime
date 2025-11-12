#include <common/command_buffers/gpu/gpu_shader_module.hpp>

using namespace std;

namespace commandbuffers
{
  GPUShaderModuleBase::GPUShaderModuleBase(Ref<GPUDeviceBase> device,
                                           const GPUShaderModuleDescriptor &descriptor,
                                           vector<wgsl::Extension> internalExtensions)
      : GPUHandle(device, descriptor.label)
  {
  }
}
