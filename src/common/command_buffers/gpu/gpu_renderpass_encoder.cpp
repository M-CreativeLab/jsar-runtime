#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>

namespace commandbuffers
{
  // static
  Ref<GPURenderPassEncoder> GPURenderPassEncoder::Create(const GPURenderPassDescriptor &descriptor,
                                                         GPUCommandEncoder *encoder,
                                                         gpu::EncodingContext *context,
                                                         uint32_t renderTargetWidth,
                                                         uint32_t renderTargetHeight,
                                                         bool depthReadOnly,
                                                         bool stencilReadOnly)
  {
    return AcquireRef(new GPURenderPassEncoder(
      nullptr,
      descriptor,
      encoder,
      context,
      renderTargetWidth,
      renderTargetHeight,
      depthReadOnly,
      stencilReadOnly));
  }
  // static
  Ref<GPURenderPassEncoder> GPURenderPassEncoder::MakeError(GPUCommandEncoder *encoder,
                                                            gpu::EncodingContext *context,
                                                            std::string_view label)
  {
    return AcquireRef(
      new GPURenderPassEncoder(
        nullptr,
        encoder,
        context,
        GPUHandle::kError,
        label));
  }

  GPURenderPassEncoder::GPURenderPassEncoder(Ref<GPUDeviceBase> device,
                                             const GPURenderPassDescriptor &descriptor,
                                             GPUCommandEncoder *commandEncoder,
                                             gpu::EncodingContext *encodingContext,
                                             uint32_t renderTargetWidth,
                                             uint32_t renderTargetHeight,
                                             bool depthReadOnly,
                                             bool stencilReadOnly)
      : GPUHandle(device, descriptor.label.value_or("GPURenderPassEncoder"))
  {
  }

  GPURenderPassEncoder::GPURenderPassEncoder(Ref<GPUDeviceBase> device,
                                             GPUCommandEncoder *commandEncoder,
                                             gpu::EncodingContext *encodingContext,
                                             ErrorTag errorTag,
                                             std::string_view label)
      : GPUHandle(device, errorTag, label)
  {
  }
}
