#include <memory>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_render_bundle.hpp>
#include <common/command_buffers/gpu/gpu_render_bundle_encoder.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

using namespace std;

namespace commandbuffers
{
  GPURenderBundle::GPURenderBundle(GPURenderBundleEncoder *encoder,
                                   const GPURenderBundleDescriptor *descriptor,
                                   Ref<gpu::AttachmentState> attachment_state,
                                   bool depth_readonly,
                                   bool stencil_readonly,
                                   gpu::RenderPassResourceUsage resource_usage,
                                   gpu::IndirectDrawMetadata indirect_draw_metadata)
      : GPUHandle(encoder->device(), kLabelNotImplemented)
      , commands_(encoder->acquireCommands())
      , attachment_state_(std::move(attachment_state))
      , indirect_draw_metadata_(std::move(indirect_draw_metadata))
      , depth_readonly_(depth_readonly)
      , stencil_readonly_(stencil_readonly)
      , resource_usage_(std::move(resource_usage))
      , encoder_label_(encoder->getLabel())
  {
  }

  GPURenderBundle::GPURenderBundle(Ref<GPUDeviceBase> device, ErrorTag error_tag, string_view label)
      : GPUHandle(device, error_tag, label)
  {
  }

  const string &GPURenderBundle::getEncoderLabel() const
  {
    return encoder_label_;
  }

  void GPURenderBundle::setEncoderLabel(string encoder_label)
  {
    encoder_label_ = encoder_label;
  }

  gpu::CommandIterator *GPURenderBundle::getCommands()
  {
    return &commands_;
  }

  const gpu::AttachmentState *GPURenderBundle::getAttachmentState() const
  {
    return attachment_state_.get();
  }

  bool GPURenderBundle::isDepthReadOnly() const
  {
    return depth_readonly_;
  }

  bool GPURenderBundle::isStencilReadOnly() const
  {
    return stencil_readonly_;
  }

  uint64_t GPURenderBundle::getDrawCount() const
  {
    return draw_count_;
  }

  const gpu::RenderPassResourceUsage &GPURenderBundle::getResourceUsage() const
  {
    return resource_usage_;
  }

  const gpu::IndirectDrawMetadata &GPURenderBundle::getIndirectDrawMetadata()
  {
    return indirect_draw_metadata_;
  }
}
