#pragma once

#include <optional>
#include <variant>

#include "./gpu_base.hpp"
#include "./gpu_buffer.hpp"
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

  class GPURenderPassEncoder : public GPUHandle
  {
    using GPUHandle::GPUHandle;

    struct Draw
    {
      uint32_t vertex_count;
      uint32_t instance_count;
      uint32_t first_vertex;
      uint32_t first_instance;
    };
    struct IndexedDraw
    {
      uint32_t index_count;
      uint32_t instance_count;
      uint32_t first_index;
      int32_t base_vertex;
      uint32_t first_instance;
    };
    using DrawCommand = std::variant<Draw, IndexedDraw>;

  public:
    virtual ~GPURenderPassEncoder() = default;

  public:
    void draw(uint32_t vertex_count,
              uint32_t instance_count = 1,
              uint32_t first_vertex = 0,
              uint32_t first_instance = 0)
    {
      if (!ended_) [[likely]]
        draw_commands_.emplace_back(Draw{vertex_count, instance_count, first_vertex, first_instance});
    }
    void drawIndexed(uint32_t index_count,
                     uint32_t instance_count = 1,
                     uint32_t first_index = 0,
                     int32_t base_vertex = 0,
                     uint32_t first_instance = 0)
    {
      if (!ended_) [[likely]]
        draw_commands_.emplace_back(IndexedDraw{index_count, instance_count, first_index, base_vertex, first_instance});
    }
    void end()
    {
      ended_ = true;
    }

  protected:
    bool ended_ = false;
    float viewport_[4] = {0.0f, 0.0f, 1.0f, 1.0f};     // x, y, width, height
    float scissor_rect_[4] = {0.0f, 0.0f, 1.0f, 1.0f}; // x, y, width, height
    std::optional<GPUBuffer> index_buffer_;
    std::optional<GPUBuffer> vertex_buffer_;
    std::optional<GPURenderPipeline> render_pipeline_;
    std::optional<float> stencil_reference_value_;
    std::vector<DrawCommand> draw_commands_;
  };
}
