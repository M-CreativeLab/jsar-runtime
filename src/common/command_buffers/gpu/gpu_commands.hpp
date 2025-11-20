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
  struct GPUCommand
  {
    enum CommandType
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
    const CommandType type;
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

  struct GPUDrawCommand
  {
    const uint32_t vertexCount;
    const uint32_t instanceCount;
    const uint32_t firstVertex;
    const uint32_t firstInstance;
  };

  struct GPUDrawIndexedCommand
  {
    const uint32_t indexCount;
    const uint32_t instanceCount;
    const uint32_t firstIndex;
    const int32_t baseVertex;
    const uint32_t firstInstance;
  };

  struct GPUSetViewportCommand
  {
    const float x;
    const float y;
    const float width;
    const float height;
    const float minDepth;
    const float maxDepth;
  };

  struct GPUSetScissorCommand
  {
    const float x;
    const float y;
    const float width;
    const float height;
  };

  struct GPUSetRenderPipelineCommand
  {
    const GPUIdentifier pipelineId;
  };

  struct GPUSetIndexBufferCommand
  {
    const GPUIdentifier bufferId;
    const GPUIndexFormat indexFormat;
    const uint32_t offset;
    const uint32_t size;
  };

  struct GPUSetVertexBufferCommand
  {
    const uint32_t slot;
    const GPUIdentifier bufferId;
    const uint32_t offset;
    const uint32_t size;
  };

  struct GPUSetBindGroupCommand
  {
    const GPUIdentifier bindGroupId;
    const uint32_t index;
  };

  struct GPUSetBlendConstantCommand
  {
    const float r;
    const float g;
    const float b;
    const float a;
  };

  struct GPUSetStencilReferenceCommand
  {
    const uint32_t reference;
  };
}
