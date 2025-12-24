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
      : gpu::RenderEncoderBase(device,
                               descriptor.label.value_or("GPURenderPassEncoder"),
                               encodingContext,
                               nullptr,
                               depthReadOnly,
                               stencilReadOnly)
  {
    command_encoder_ = commandEncoder;
    render_target_width_ = renderTargetWidth;
    render_target_height_ = renderTargetHeight;
  }

  GPURenderPassEncoder::GPURenderPassEncoder(Ref<GPUDeviceBase> device,
                                             GPUCommandEncoder *commandEncoder,
                                             gpu::EncodingContext *encodingContext,
                                             ErrorTag errorTag,
                                             std::string_view label)
      : gpu::RenderEncoderBase(device, encodingContext, errorTag, label)
  {
    command_encoder_ = commandEncoder;
  }

  void GPURenderPassEncoder::setViewport(float x,
                                         float y,
                                         float width,
                                         float height,
                                         float min_depth,
                                         float max_depth)
  {
    encoding_context_->tryEncode(
      this,
      [&](gpu::CommandAllocator *allocator) -> gpu::MaybeError
      {
        GPUSetViewportCommand *cmd = allocator->allocate<GPUSetViewportCommand>(GPUCommand::kSetViewport);
        cmd->x = x;
        cmd->y = y;
        cmd->width = width;
        cmd->height = height;
        cmd->minDepth = min_depth;
        cmd->maxDepth = max_depth;
        return {};
      });
  }

  void GPURenderPassEncoder::setScissorRect(float x, float y, float width, float height)
  {
    encoding_context_->tryEncode(
      this,
      [&](gpu::CommandAllocator *allocator) -> gpu::MaybeError
      {
        GPUSetScissorCommand *cmd = allocator->allocate<GPUSetScissorCommand>(GPUCommand::kSetScissorRect);
        cmd->x = x;
        cmd->y = y;
        cmd->width = width;
        cmd->height = height;
        return {};
      });
  }

  void GPURenderPassEncoder::setPipeline(const GPURenderPipelineBase &pipeline)
  {
    gpu::RenderEncoderBase::setPipeline(const_cast<GPURenderPipelineBase *>(&pipeline));
  }

  void GPURenderPassEncoder::setIndexBuffer(const GPUBufferBase &buffer,
                                            GPUIndexFormat index_format,
                                            uint32_t offset,
                                            uint32_t size)
  {
    gpu::RenderEncoderBase::setIndexBuffer(const_cast<GPUBufferBase *>(&buffer),
                                           index_format,
                                           static_cast<uint64_t>(offset),
                                           static_cast<uint64_t>(size));
  }

  void GPURenderPassEncoder::setVertexBuffer(uint32_t slot,
                                             const GPUBufferBase &buffer,
                                             uint32_t offset,
                                             uint32_t size)
  {
    gpu::RenderEncoderBase::setVertexBuffer(slot,
                                            const_cast<GPUBufferBase *>(&buffer),
                                            static_cast<uint64_t>(offset),
                                            static_cast<uint64_t>(size));
  }

  void GPURenderPassEncoder::setBlendConstant(float r, float g, float b, float a)
  {
    encoding_context_->tryEncode(
      this,
      [&](gpu::CommandAllocator *allocator) -> gpu::MaybeError
      {
        GPUSetBlendConstantCommand *cmd = allocator->allocate<GPUSetBlendConstantCommand>(GPUCommand::kSetBlendConstant);
        cmd->r = r;
        cmd->g = g;
        cmd->b = b;
        cmd->a = a;
        return {};
      });
  }

  void GPURenderPassEncoder::setStencilReference(uint32_t ref)
  {
    encoding_context_->tryEncode(
      this,
      [&](gpu::CommandAllocator *allocator) -> gpu::MaybeError
      {
        GPUSetStencilReferenceCommand *cmd = allocator->allocate<GPUSetStencilReferenceCommand>(GPUCommand::kSetStencilReference);
        cmd->reference = ref;
        return {};
      });
  }

  void GPURenderPassEncoder::end()
  {
    if (this->gpu::ProgrammableEncoder::ended_ && isValidationEnabled())
    {
      if (encoding_context_ != nullptr)
      {
        encoding_context_->handleError(gpu::ErrorData::Create(
          gpu::InternalErrorType::kValidation,
          "GPURenderPassEncoder was already ended.",
          __FILE__,
          __func__,
          __LINE__));
      }
      return;
    }

    this->gpu::ProgrammableEncoder::ended_ = true;
    GPUPassEncoderBase::end();

    encoding_context_->tryEncode(
      this,
      [&](gpu::CommandAllocator *allocator) -> gpu::MaybeError
      {
        if (isValidationEnabled())
        {
          gpu::MaybeError validation = validateProgrammableEncoderEnd();
          if (validation.IsError())
          {
            return validation;
          }
        }

        allocator->allocate<GPUEndRenderPassCommand>(GPUCommand::kEndRenderPass);

        gpu::RenderPassResourceUsageTracker usage_tracker;
        encoding_context_->exitRenderPass(this,
                                          std::move(usage_tracker),
                                          command_encoder_,
                                          std::move(indirect_draw_metadata_));
        return {};
      });
  }
}
