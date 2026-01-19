#pragma once

#include <string>
#include <string_view>
#include <common/command_buffers/gpu/attachment_set.hpp>
#include <common/command_buffers/gpu/command_allocator.hpp>
#include <common/command_buffers/gpu/pass_resource_usage.hpp>
#include <common/command_buffers/gpu/indirect_draw_metadata.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;
  class GPURenderBundleEncoder;
  struct GPURenderBundleDescriptor
  {
    std::string_view label;
  };

  class GPURenderBundle final : public GPUHandle
  {
  public:
    GPURenderBundle(GPURenderBundleEncoder *encoder,
                    const GPURenderBundleDescriptor *descriptor,
                    Ref<gpu::AttachmentState> attachment_state,
                    bool depth_readonly,
                    bool stencil_readonly,
                    gpu::RenderPassResourceUsage resource_usage,
                    gpu::IndirectDrawMetadata indirect_draw_metadata);

    GPUHandleType type() const override
    {
      return GPUHandleType::kRenderBundle;
    }

    const std::string &getEncoderLabel() const;
    void setEncoderLabel(std::string);

    gpu::CommandIterator *getCommands();

    const gpu::AttachmentState *getAttachmentState() const;
    bool isDepthReadOnly() const;
    bool isStencilReadOnly() const;
    uint64_t getDrawCount() const;
    const gpu::RenderPassResourceUsage &getResourceUsage() const;
    const gpu::IndirectDrawMetadata &getIndirectDrawMetadata();

  private:
    GPURenderBundle(Ref<GPUDeviceBase> device, ErrorTag error_tag, std::string_view label);

    gpu::CommandIterator commands_;
    gpu::IndirectDrawMetadata indirect_draw_metadata_;
    Ref<gpu::AttachmentState> attachment_state_;
    bool depth_readonly_;
    bool stencil_readonly_;
    uint64_t draw_count_;
    gpu::RenderPassResourceUsage resource_usage_;
    std::string encoder_label_;
  };
}
