#pragma once

#include <optional>
#include <variant>

#include "./gpu_base.hpp"
#include "./gpu_pass_encoder_base.hpp"
#include "./gpu_buffer.hpp"
#include "./gpu_command_buffer.hpp"
#include "./gpu_pipeline.hpp"
#include "./gpu_texture_view.hpp"

namespace commandbuffers
{
  class GPURenderPassDescriptor
  {
  public:
    enum LoadOp
    {
      Load,
      Clear,
    };
    enum StoreOp
    {
      Store,
      Discard,
    };

    class ColorAttachment
    {
    public:
      float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
      LoadOp loadOp;
      StoreOp storeOp;
      std::optional<GPUTextureView> resolveTarget;
      GPUTextureView view;
    };

    class DepthStencilAttachment
    {
    public:
      float depthClearValue = 1.0f;
      std::optional<bool> depthReadOnly;
      std::optional<LoadOp> depthLoadOp;
      std::optional<StoreOp> depthStoreOp;
      float stencilClearValue = 0.0f;
      std::optional<bool> stencilReadOnly;
      std::optional<LoadOp> stencilLoadOp;
      std::optional<StoreOp> stencilStoreOp;
      GPUTextureView view;
    };

  public:
    std::optional<std::string> label;
    std::optional<uint32_t> maxDrawCount;
    std::vector<ColorAttachment> colorAttachments;
    std::optional<DepthStencilAttachment> depthStencilAttachment;
  };

  class GPURenderPassEncoder : public GPUPassEncoderBase,
                               public GPUHandle
  {
  public:
    GPURenderPassEncoder(std::string label)
        : GPUPassEncoderBase()
        , GPUHandle(label)
    {
    }

  public:
    bool isRenderPassEncoder() const override
    {
      return true;
    }

    void draw(uint32_t vertex_count,
              uint32_t instance_count = 1,
              uint32_t first_vertex = 0,
              uint32_t first_instance = 0)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUDrawCommand>(vertex_count,
                                                    instance_count,
                                                    first_vertex,
                                                    first_instance);
    }
    void drawIndexed(uint32_t index_count,
                     uint32_t instance_count = 1,
                     uint32_t first_index = 0,
                     int32_t base_vertex = 0,
                     uint32_t first_instance = 0)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUDrawIndexedCommand>(index_count,
                                                           instance_count,
                                                           first_index,
                                                           base_vertex,
                                                           first_instance);
    }
    void setViewport(float x, float y, float width, float height, float min_depth = 0.0f, float max_depth = 1.0f)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUSetViewportCommand>(x, y, width, height, min_depth, max_depth);
    }
    void setScissorRect(float x, float y, float width, float height)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUSetScissorCommand>(x, y, width, height);
    }
    void setPipeline(const GPURenderPipeline &pipeline)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUSetRenderPipelineCommand>(pipeline);
    }
    void setIndexBuffer(const GPUBuffer &buffer, GPUIndexFormat index_format, uint32_t offset, uint32_t size)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUSetIndexBufferCommand>(buffer, index_format, offset, size);
    }
    void setVertexBuffer(uint32_t slot, const GPUBuffer &buffer, uint32_t offset = 0, uint32_t size = 0)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUSetVertexBufferCommand>(slot, buffer, offset, size);
    }
    void setBlendConstant(float r, float g, float b, float a)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUSetBlendConstantCommand>(r, g, b, a);
    }
    void setStencilReference(uint32_t ref)
    {
      if (!ended_) [[likely]]
        command_buffer_->addCommand<GPUSetStencilReferenceCommand>(ref);
    }
  };
}
