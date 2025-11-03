#pragma once

#include <optional>
#include <variant>

#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_pass_encoder_base.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_texture_view.hpp>
#include <common/command_buffers/gpu/gpu_commands.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>

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
      std::weak_ptr<GPUTextureViewBase> resolveTarget;
      std::weak_ptr<GPUTextureViewBase> view;
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
      std::weak_ptr<GPUTextureViewBase> view;
    };

  public:
    std::optional<std::string> label;
    std::optional<uint32_t> maxDrawCount;
    std::vector<ColorAttachment> colorAttachments;
    std::optional<DepthStencilAttachment> depthStencilAttachment;
  };

  class GPURenderPassEncoderBase : public GPUPassEncoderBase,
                                   public GPUHandle
  {
  public:
    GPUHandleType type() const override final
    {
      return GPUHandleType::kRenderPassEncoder;
    }
    bool isRenderPassEncoder() const override
    {
      return true;
    }

    void draw(
      uint32_t vertex_count,
      uint32_t instance_count = 1,
      uint32_t first_vertex = 0,
      uint32_t first_instance = 0);
    void drawIndexed(
      uint32_t index_count,
      uint32_t instance_count = 1,
      uint32_t first_index = 0,
      int32_t base_vertex = 0,
      uint32_t first_instance = 0);
    void setViewport(float x, float y, float width, float height, float min_depth = 0.0f, float max_depth = 1.0f);
    void setScissorRect(float x, float y, float width, float height);
    void setPipeline(const GPURenderPipelineBase &pipeline);
    void setIndexBuffer(const GPUBufferBase &buffer, GPUIndexFormat index_format, uint32_t offset, uint32_t size);
    void setVertexBuffer(uint32_t slot, const GPUBufferBase &buffer, uint32_t offset = 0, uint32_t size = 0);
    void setBlendConstant(float r, float g, float b, float a);
    void setStencilReference(uint32_t ref);
  };
}
