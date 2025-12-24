#pragma once

#include <string_view>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/attachment_set.hpp>
#include <common/command_buffers/gpu/programmable_encoder.hpp>
#include <common/command_buffers/gpu/encoding_context.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_render_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_bind_group.hpp>
#include "command_buffers/gpu/indirect_draw_metadata.hpp"

namespace commandbuffers
{
  class GPUDeviceBase;

  namespace gpu
  {
    class RenderEncoderBase : public ProgrammableEncoder
    {
    public:
      RenderEncoderBase(Ref<GPUDeviceBase> device,
                        std::string_view label,
                        EncodingContext *encoding_context,
                        Ref<AttachmentState> attachment_state,
                        bool depth_readonly,
                        bool stencil_readonly);

      void draw(uint32_t vertex_count,
                uint32_t instance_count = 1,
                uint32_t first_vertex = 0,
                uint32_t first_instance = 0);
      void drawIndexed(uint32_t index_count,
                       uint32_t instance_count = 1,
                       uint32_t first_index = 0,
                       int32_t base_vertex = 0,
                       uint32_t first_instance = 0);

      void drawIndirect(GPUBufferBase *indirect_buffer, uint64_t indirect_fffset);
      void drawIndexedIndirect(GPUBufferBase *indirect_buffer, uint64_t indirect_fffset);

      void multiDrawIndirect(
        GPUBufferBase *indirect_buffer,
        uint64_t indirect_fffset,
        uint32_t max_draw_count,
        GPUBufferBase *draw_count_buffer = nullptr,
        uint64_t draw_count_buffer_offset = 0);
      void multiDrawIndexedIndirect(
        GPUBufferBase *indirect_buffer,
        uint64_t indirect_fffset,
        uint32_t max_draw_count,
        GPUBufferBase *draw_count_buffer = nullptr,
        uint64_t draw_count_buffer_offset = 0);

      void setPipeline(GPURenderPipelineBase *pipeline);
      void setVertexBuffer(uint32_t slot, GPUBufferBase *buffer, uint64_t offset, uint64_t size);
      void setIndexBuffer(GPUBufferBase *buffer,
                          GPUIndexFormat format,
                          uint64_t offset,
                          uint64_t size);

      void setBindGroup(uint32_t group_index,
                        GPUBindGroupBase *group,
                        uint32_t dynamic_offset_count = 0,
                        const uint32_t *dynamic_offsets = nullptr);

      const AttachmentState *getAttachmentState() const;
      bool isDepthReadOnly() const;
      bool isStencilReadOnly() const;
      uint64_t getDrawCount() const;
      Ref<AttachmentState> acquireAttachmentState();

    protected:
      // Construct an "error" render encoder base.
      RenderEncoderBase(Ref<GPUDeviceBase> device,
                        EncodingContext *encoding_context,
                        ErrorTag error_tag,
                        std::string_view label);

      IndirectDrawMetadata indirect_draw_metadata_;
      uint64_t draw_count_ = 0;

    private:
      Ref<AttachmentState> attachment_state_;
      const bool disable_base_vertex_;
      const bool disable_base_instance_;
      bool depth_read_only_ = false;
      bool stencil_read_only_ = false;
    };
  }
}
