#pragma once

#include <vector>
#include <memory>

#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_render_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_compute_pipeline.hpp>

namespace commandbuffers
{
  enum class GPUCommand : uint32_t
  {
    kBeginComputePass,
    kBeginOcclusionQuery,
    kBeginRenderPass,
    kClearBuffer,
    kCopyBufferToBuffer,
    kCopyBufferToTexture,
    kCopyTextureToBuffer,
    kCopyTextureToTexture,
    kDispatch,
    kDispatchIndirect,
    kDraw,
    kDrawIndexed,
    kDrawIndirect,
    kDrawIndexedIndirect,
    kMultiDrawIndirect,
    kMultiDrawIndexedIndirect,
    kEndComputePass,
    kEndOcclusionQuery,
    kEndRenderPass,
    kExecuteBundles,
    kInsertDebugMarker,
    kPixelLocalStorageBarrier,
    kPopDebugGroup,
    kPushDebugGroup,
    kResolveQuerySet,
    kSetComputePipeline,
    kSetRenderPipeline,
    kSetStencilReference,
    kSetViewport,
    kSetScissorRect,
    kSetBlendConstant,
    kSetBindGroup,
    kSetImmediateData,
    kSetIndexBuffer,
    kSetVertexBuffer,
    kWriteBuffer,
    kWriteTimestamp,
  };

  struct GPUInsertDebugMarkerCommand
  {
    uint32_t length;
  };


  struct GPUBeginRenderPassCommand
  {
    // Ref<AttachmentState> attachmentState;
    // PerColorAttachment<RenderPassColorAttachmentInfo> colorAttachments;
    // RenderPassDepthStencilAttachmentInfo depthStencilAttachment;

    // std::array<RenderPassStorageAttachmentInfo, kMaxPLSSlots> storageAttachments;

    // Cache the width and height of all attachments for convenience
    uint32_t width;
    uint32_t height;
    // Used for partial resolve
    // ResolveRect resolveRect;

    // Ref<QuerySetBase> occlusionQuerySet;
    // TimestampWrites timestampWrites;
    std::string label;
  };

  struct GPUEndRenderPassCommand
  {
  };

  struct GPUDrawCommand
  {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
  };

  struct GPUDrawIndexedCommand
  {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t baseVertex;
    uint32_t firstInstance;
  };

  struct GPUSetViewportCommand
  {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
  };

  struct GPUSetScissorCommand
  {
    float x;
    float y;
    float width;
    float height;
  };

  struct GPUSetRenderPipelineCommand
  {
    GPUIdentifier pipelineId;
  };

  struct GPUSetIndexBufferCommand
  {
    GPUIdentifier bufferId;
    GPUIndexFormat indexFormat;
    uint32_t offset;
    uint32_t size;
  };

  struct GPUSetVertexBufferCommand
  {
    uint32_t slot;
    GPUIdentifier bufferId;
    uint32_t offset;
    uint32_t size;
  };

  struct GPUSetBindGroupCommand
  {
    GPUIdentifier bindGroupId;
    uint32_t index;
  };

  struct GPUSetBlendConstantCommand
  {
    float r;
    float g;
    float b;
    float a;
  };

  struct GPUSetStencilReferenceCommand
  {
    uint32_t reference;
  };
}
