#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_commands.hpp>
#include <common/command_buffers/gpu/render_encoder_base.hpp>
#include "command_buffers/gpu/indirect_draw_metadata.hpp"

namespace commandbuffers::gpu
{
  using namespace std;

  RenderEncoderBase::RenderEncoderBase(Ref<GPUDeviceBase> device,
                                       string_view label,
                                       EncodingContext *encoding_context,
                                       Ref<AttachmentState> attachment_state,
                                       bool depth_readonly,
                                       bool stencil_readonly)
      : ProgrammableEncoder(device, label, encoding_context)
      , attachment_state_(std::move(attachment_state))
      , disable_base_vertex_(true)
      , disable_base_instance_(true)
  {
    depth_read_only_ = depth_readonly;
    stencil_read_only_ = stencil_readonly;
  }

  RenderEncoderBase::RenderEncoderBase(Ref<GPUDeviceBase> device,
                                       EncodingContext *encoding_context,
                                       ErrorTag error_tag,
                                       std::string_view label)
      : ProgrammableEncoder(device, encoding_context, error_tag, label)
      , disable_base_vertex_(true)
      , disable_base_instance_(true)
  {
  }

  void RenderEncoderBase::draw(uint32_t vertex_count,
                               uint32_t instance_count,
                               uint32_t first_vertex,
                               uint32_t first_instance)
  {
    encoding_context_->tryEncode(
      this,
      [&](CommandAllocator *allocator) -> MaybeError
      {
        auto *draw_command = allocator->allocate<GPUDrawCommand>(GPUCommand::kDraw);
        draw_command->vertexCount = vertex_count;
        draw_command->instanceCount = instance_count;
        draw_command->firstVertex = first_vertex;
        draw_command->firstInstance = first_instance;
        draw_count_++;
        return {};
      });
  }

  void RenderEncoderBase::drawIndexed(uint32_t index_count,
                                      uint32_t instance_count,
                                      uint32_t first_index,
                                      int32_t base_vertex,
                                      uint32_t first_instance)
  {
    encoding_context_->tryEncode(
      this,
      [&](CommandAllocator *allocator) -> MaybeError
      {
        auto *cmd = allocator->allocate<GPUDrawIndexedCommand>(GPUCommand::kDrawIndexed);
        cmd->indexCount = index_count;
        cmd->instanceCount = instance_count;
        cmd->firstIndex = first_index;
        cmd->baseVertex = base_vertex;
        cmd->firstInstance = first_instance;
        draw_count_++;
        return {};
      });
  }

  void RenderEncoderBase::drawIndirect(GPUBufferBase *indirect_buffer, uint64_t indirect_fffset)
  {
  }

  void RenderEncoderBase::drawIndexedIndirect(GPUBufferBase *indirect_buffer, uint64_t indirect_fffset)
  {
  }

  void RenderEncoderBase::multiDrawIndirect(
    GPUBufferBase *indirect_buffer,
    uint64_t indirect_fffset,
    uint32_t max_draw_count,
    GPUBufferBase *draw_count_buffer,
    uint64_t draw_count_buffer_offset)
  {
  }

  void RenderEncoderBase::multiDrawIndexedIndirect(
    GPUBufferBase *indirect_buffer,
    uint64_t indirect_fffset,
    uint32_t max_draw_count,
    GPUBufferBase *draw_count_buffer,
    uint64_t draw_count_buffer_offset)
  {
  }

  void RenderEncoderBase::setPipeline(GPURenderPipelineBase *pipeline)
  {
    if (pipeline == nullptr)
    {
      return;
    }
    encoding_context_->tryEncode(
      this,
      [&](CommandAllocator *allocator) -> MaybeError
      {
        auto *cmd = allocator->allocate<GPUSetRenderPipelineCommand>(GPUCommand::kSetRenderPipeline);
        cmd->pipelineId = pipeline->id;
        return {};
      });
  }

  void RenderEncoderBase::setVertexBuffer(uint32_t slot, GPUBufferBase *buffer, uint64_t offset, uint64_t size)
  {
    if (buffer == nullptr)
    {
      return;
    }
    encoding_context_->tryEncode(
      this,
      [&](CommandAllocator *allocator) -> MaybeError
      {
        auto *cmd = allocator->allocate<GPUSetVertexBufferCommand>(GPUCommand::kSetVertexBuffer);
        cmd->slot = slot;
        cmd->bufferId = buffer->id;
        cmd->offset = static_cast<uint32_t>(offset);
        cmd->size = static_cast<uint32_t>(size);
        return {};
      });
  }

  void RenderEncoderBase::setIndexBuffer(GPUBufferBase *buffer,
                                         GPUIndexFormat format,
                                         uint64_t offset,
                                         uint64_t size)
  {
    if (buffer == nullptr)
    {
      return;
    }
    encoding_context_->tryEncode(
      this,
      [&](CommandAllocator *allocator) -> MaybeError
      {
        auto *cmd = allocator->allocate<GPUSetIndexBufferCommand>(GPUCommand::kSetIndexBuffer);
        cmd->bufferId = buffer->id;
        cmd->indexFormat = format;
        cmd->offset = static_cast<uint32_t>(offset);
        cmd->size = static_cast<uint32_t>(size);
        return {};
      });
  }

  void RenderEncoderBase::setBindGroup(uint32_t group_index,
                                       GPUBindGroupBase *group,
                                       uint32_t dynamic_offset_count,
                                       const uint32_t *dynamic_offsets)
  {
  }

  const AttachmentState *RenderEncoderBase::getAttachmentState() const
  {
    return attachment_state_.get();
  }

  bool RenderEncoderBase::isDepthReadOnly() const
  {
    return depth_read_only_;
  }

  bool RenderEncoderBase::isStencilReadOnly() const
  {
    return stencil_read_only_;
  }

  uint64_t RenderEncoderBase::getDrawCount() const
  {
    return draw_count_;
  }

  Ref<AttachmentState> RenderEncoderBase::acquireAttachmentState()
  {
    return std::move(attachment_state_);
  }
}
