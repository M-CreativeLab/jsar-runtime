#pragma once

#include <vector>
#include <memory>

#include "./gpu_base.hpp"
#include "./gpu_buffer.hpp"
#include "./gpu_pipeline.hpp"

namespace commandbuffers
{
  class GPUCommand
  {
  public:
    enum GPUCommandType : uint8_t
    {
      kDraw = 0,
      kDrawIndexed,
      kDrawIndirect,
      kDrawIndexedIndirect,
      kSetViewport,
      kSetScissor,
      kSetRenderPipeline,
      kSetIndexBuffer,
      kSetVertexBuffer,
      kSetBindGroup,
      kSetBlendConstant,
      kSetStencilReference,
    };

  public:
    const GPUCommandType type;

    GPUCommand(GPUCommandType type)
        : type(type)
    {
    }
    virtual ~GPUCommand() = default;
  };

  class GPUDrawCommand : public GPUCommand
  {
  public:
    const uint32_t vertexCount;
    const uint32_t instanceCount;
    const uint32_t firstVertex;
    const uint32_t firstInstance;

    GPUDrawCommand(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
        : GPUCommand(kDraw)
        , vertexCount(vertex_count)
        , instanceCount(instance_count)
        , firstVertex(first_vertex)
        , firstInstance(first_instance)
    {
    }
  };

  class GPUDrawIndexedCommand : public GPUCommand
  {
  public:
    const uint32_t indexCount;
    const uint32_t instanceCount;
    const uint32_t firstIndex;
    const int32_t baseVertex;
    const uint32_t firstInstance;

    GPUDrawIndexedCommand(uint32_t index_count,
                          uint32_t instance_count,
                          uint32_t first_index,
                          int32_t base_vertex,
                          uint32_t first_instance)
        : GPUCommand(kDrawIndexed)
        , indexCount(index_count)
        , instanceCount(instance_count)
        , firstIndex(first_index)
        , baseVertex(base_vertex)
        , firstInstance(first_instance)
    {
    }
  };

  class GPUSetViewportCommand : public GPUCommand
  {
  public:
    const float x;
    const float y;
    const float width;
    const float height;
    const float minDepth;
    const float maxDepth;

    GPUSetViewportCommand(float x, float y, float width, float height, float min_depth, float max_depth)
        : GPUCommand(kSetViewport)
        , x(x)
        , y(y)
        , width(width)
        , height(height)
        , minDepth(min_depth)
        , maxDepth(max_depth)
    {
    }
  };

  class GPUSetScissorCommand : public GPUCommand
  {
  public:
    const float x;
    const float y;
    const float width;
    const float height;

    GPUSetScissorCommand(float x, float y, float width, float height)
        : GPUCommand(kSetScissor)
        , x(x)
        , y(y)
        , width(width)
        , height(height)
    {
    }
  };

  class GPUSetRenderPipelineCommand : public GPUCommand
  {
  public:
    const GPUIdentifier pipelineId;

    GPUSetRenderPipelineCommand(const GPURenderPipeline &pipeline)
        : GPUCommand(kSetRenderPipeline)
        , pipelineId(pipeline.id)
    {
    }
  };

  class GPUSetIndexBufferCommand : public GPUCommand
  {
  public:
    const GPUIdentifier bufferId;
    const GPUIndexFormat indexFormat;
    const uint32_t offset;
    const uint32_t size;

    GPUSetIndexBufferCommand(const GPUBuffer &buffer, GPUIndexFormat index_format, uint32_t offset, uint32_t size)
        : GPUCommand(kSetIndexBuffer)
        , bufferId(buffer.id)
        , indexFormat(index_format)
        , offset(offset)
        , size(size)
    {
    }
  };

  class GPUSetVertexBufferCommand : public GPUCommand
  {
  public:
    const uint32_t slot;
    const GPUIdentifier bufferId;
    const uint32_t offset;
    const uint32_t size;

    GPUSetVertexBufferCommand(const uint32_t slot, const GPUBuffer &buffer, uint64_t offset, uint32_t size)
        : GPUCommand(kSetVertexBuffer)
        , slot(slot)
        , bufferId(buffer.id)
        , offset(offset)
        , size(size)
    {
    }
  };

  class GPUSetBindGroupCommand : public GPUCommand
  {
  public:
    const GPUIdentifier bindGroupId;
    const uint32_t index;

    GPUSetBindGroupCommand(const GPUBindGroup &bindGroup, uint32_t index)
        : GPUCommand(kSetBindGroup)
        , bindGroupId(bindGroup.id)
        , index(index)
    {
    }
  };

  class GPUSetBlendConstantCommand : public GPUCommand
  {
  public:
    const float r;
    const float g;
    const float b;
    const float a;

    GPUSetBlendConstantCommand(float r, float g, float b, float a)
        : GPUCommand(kSetBlendConstant)
        , r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }
  };

  class GPUSetStencilReferenceCommand : public GPUCommand
  {
  public:
    const uint32_t reference;

    GPUSetStencilReferenceCommand(uint32_t reference)
        : GPUCommand(kSetStencilReference)
        , reference(reference)
    {
    }
  };

  class GPUCommandBuffer : public GPUHandle
  {
    friend class GPUComputePassEncoder;
    friend class GPURenderPassEncoder;

  public:
    GPUCommandBuffer(std::optional<std::string> label)
        : GPUHandle(label.value_or("GPUCommandBuffer"))
        , commands_()
    {
    }
    GPUCommandBuffer(std::optional<std::string> label, const GPUCommandBuffer &source)
        : GPUHandle(label.value_or("GPUCommandBuffer"))
        , commands_(source.commands_)
    {
    }
    virtual ~GPUCommandBuffer() = default;

  public:
    virtual void execute() = 0;

  private:
    template <typename T, typename... Args>
    void addCommand(Args &&...args)
    {
      auto command = std::make_shared<T>(std::forward<Args>(args)...);
      commands_.push_back(command);
    }

  protected:
    std::vector<std::shared_ptr<GPUCommand>> commands_;
  };
}
