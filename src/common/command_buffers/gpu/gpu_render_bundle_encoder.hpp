#pragma once

#include <string_view>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/command_allocator.hpp>
#include <common/command_buffers/gpu/pass_resource_usage.hpp>
#include <common/command_buffers/gpu/render_encoder_base.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;
  class GPURenderBundle;

  struct GPURenderBundleEncoderDescriptor
  {
    std::string_view label;
    size_t colorFormatCount;
    GPUTextureFormat const *colorFormats = nullptr;
    GPUTextureFormat depthStencilFormat = GPUTextureFormat::kUndefined;
    uint32_t sampleCount = 1;
    bool depthReadOnly = false;
    bool stencilReadOnly = false;
  };

  class GPURenderBundleEncoder final : public gpu::RenderEncoderBase
  {
  public:
    GPUHandleType type() const override
    {
      return GPUHandleType::kRenderBundleEncoder;
    }

    Ref<GPURenderBundle> finish(const GPURenderBundleEncoderDescriptor *descriptor);
    gpu::CommandIterator acquireCommands();

  private:
    GPURenderBundleEncoder(Ref<GPUDeviceBase> device, const GPURenderBundleEncoderDescriptor *descriptor);
    GPURenderBundleEncoder(Ref<GPUDeviceBase> device, ErrorTag error_tag, std::string_view label);

    gpu::ResultOrError<Ref<GPURenderBundle>> finishInternal(const GPURenderBundleEncoderDescriptor *descriptor);
    gpu::MaybeError validateFinish(const gpu::RenderPassResourceUsage &usages) const;

    gpu::EncodingContext bundle_encoding_context_;
  };
}
