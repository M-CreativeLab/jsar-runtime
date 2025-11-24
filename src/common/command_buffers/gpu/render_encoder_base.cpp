#include <common/command_buffers/gpu/gpu_device.hpp>
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
  }

  void RenderEncoderBase::drawIndexed(uint32_t vertex_count,
                                      uint32_t instance_count,
                                      uint32_t first_index,
                                      int32_t base_vertex,
                                      uint32_t first_instance)
  {
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
  }

  void RenderEncoderBase::setVertexBuffer(uint32_t slot, GPUBufferBase *buffer, uint64_t offset, uint64_t size)
  {
  }

  void RenderEncoderBase::setIndexBuffer(GPUBufferBase *buffer,
                                         GPUIndexFormat format,
                                         uint64_t offset,
                                         uint64_t size)
  {
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