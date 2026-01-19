#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_render_pipeline.hpp>

using namespace std;

namespace commandbuffers
{
  GPURenderPipelineBase::GPURenderPipelineBase(Ref<GPUDeviceBase> device,
                                               const GPURenderPipelineDescriptor &descriptor)
      : GPUPipelineBase(device, descriptor.layout, descriptor.label)
  {
  }
}
