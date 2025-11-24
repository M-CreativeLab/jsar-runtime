#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_render_bundle_encoder.hpp>
#include <common/command_buffers/gpu/gpu_render_bundle.hpp>

namespace commandbuffers
{
  GPURenderBundleEncoder::GPURenderBundleEncoder(Ref<GPUDeviceBase> device,
                                                 const GPURenderBundleEncoderDescriptor *descriptor)
      : RenderEncoderBase(device,
                          descriptor->label,
                          &bundle_encoding_context_,
                          nullptr,
                          descriptor->depthReadOnly,
                          descriptor->stencilReadOnly)
      , bundle_encoding_context_(device, this)
  {
  }

  GPURenderBundleEncoder::GPURenderBundleEncoder(Ref<GPUDeviceBase> device,
                                                 ErrorTag error_tag,
                                                 std::string_view label)
      : RenderEncoderBase(device, &bundle_encoding_context_, error_tag, label)
      , bundle_encoding_context_(device, error_tag)
  {
  }

  Ref<GPURenderBundle> GPURenderBundleEncoder::finish(const GPURenderBundleEncoderDescriptor *descriptor)
  {
    assert(false && "GPURenderBundleEncoder::finish is not implemented");
  }

  gpu::CommandIterator GPURenderBundleEncoder::acquireCommands()
  {
    return bundle_encoding_context_.acquireCommands();
  }

  gpu::ResultOrError<Ref<GPURenderBundle>> GPURenderBundleEncoder::finishInternal(const GPURenderBundleEncoderDescriptor *descriptor)
  {
    assert(false && "GPURenderBundleEncoder::finishInternal is not implemented");
  }

  gpu::MaybeError GPURenderBundleEncoder::validateFinish(const gpu::RenderPassResourceUsage &usages) const
  {
    return {};
  }
}
