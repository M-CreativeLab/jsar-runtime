#include "./common.hpp"
#include "./framebuffer.hpp"
#include "./gpu_command_buffer_impl.hpp"
#include "command_buffers/gpu/gpu_commands.hpp"

namespace gles
{
  using namespace std;
  using namespace commandbuffers;

  GPUCommandBufferImpl::GPUCommandBufferImpl(GPUCommandEncoder *encoder, const GPUCommandBufferDescriptor *descriptor)
      : GPUCommandBufferBase(encoder, descriptor)
  {
  }

  void GPUCommandBufferImpl::execute()
  {
    GPUCommand type;
    while (commands_.nextCommandId(&type))
    {
      switch (type)
      {
      case GPUCommand::kBeginComputePass:
        std::cout << "GPUCommandBufferImpl::execute: kBeginComputePass" << std::endl;
        break;
      case GPUCommand::kBeginOcclusionQuery:
        std::cout << "GPUCommandBufferImpl::execute: kBeginOcclusionQuery" << std::endl;
        break;
      case GPUCommand::kBeginRenderPass:
      {
        GPUBeginRenderPassCommand *cmd = commands_.nextCommand<GPUBeginRenderPassCommand>();
        assert(cmd != nullptr && "GPUBeginRenderPassCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kBeginRenderPass ("
                  << "height=" << cmd->height << ", "
                  << "width=" << cmd->width << ")"
                  << " " << cmd->label
                  << std::endl;
        break;
      }
      case GPUCommand::kClearBuffer:
        std::cout << "GPUCommandBufferImpl::execute: kClearBuffer" << std::endl;
        break;
      case GPUCommand::kCopyBufferToBuffer:
        std::cout << "GPUCommandBufferImpl::execute: kCopyBufferToBuffer" << std::endl;
        break;
      case GPUCommand::kCopyBufferToTexture:
        std::cout << "GPUCommandBufferImpl::execute: kCopyBufferToTexture" << std::endl;
        break;
      case GPUCommand::kCopyTextureToBuffer:
        std::cout << "GPUCommandBufferImpl::execute: kCopyTextureToBuffer" << std::endl;
        break;
      case GPUCommand::kCopyTextureToTexture:
        std::cout << "GPUCommandBufferImpl::execute: kCopyTextureToTexture" << std::endl;
        break;
      case GPUCommand::kDispatch:
        std::cout << "GPUCommandBufferImpl::execute: kDispatch" << std::endl;
        break;
      case GPUCommand::kDispatchIndirect:
        std::cout << "GPUCommandBufferImpl::execute: kDispatchIndirect" << std::endl;
        break;
      case GPUCommand::kDraw:
      {
        GPUDrawCommand *cmd = commands_.nextCommand<GPUDrawCommand>();
        assert(cmd != nullptr && "GPUDrawCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kDraw ("
                  << "vertexCount=" << cmd->vertexCount << ", "
                  << "instanceCount=" << cmd->instanceCount << ", "
                  << "firstVertex=" << cmd->firstVertex << ", "
                  << "firstInstance=" << cmd->firstInstance << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kDrawIndexed:
      {
        GPUDrawIndexedCommand *cmd = commands_.nextCommand<GPUDrawIndexedCommand>();
        assert(cmd != nullptr && "GPUDrawIndexedCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kDrawIndexed ("
                  << "indexCount=" << cmd->indexCount << ", "
                  << "instanceCount=" << cmd->instanceCount << ", "
                  << "firstIndex=" << cmd->firstIndex << ", "
                  << "baseVertex=" << cmd->baseVertex << ", "
                  << "firstInstance=" << cmd->firstInstance << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kDrawIndirect:
        std::cout << "GPUCommandBufferImpl::execute: kDrawIndirect" << std::endl;
        break;
      case GPUCommand::kDrawIndexedIndirect:
        std::cout << "GPUCommandBufferImpl::execute: kDrawIndexedIndirect" << std::endl;
        break;
      case GPUCommand::kMultiDrawIndirect:
        std::cout << "GPUCommandBufferImpl::execute: kMultiDrawIndirect" << std::endl;
        break;
      case GPUCommand::kMultiDrawIndexedIndirect:
        std::cout << "GPUCommandBufferImpl::execute: kMultiDrawIndexedIndirect" << std::endl;
        break;
      case GPUCommand::kEndComputePass:
        std::cout << "GPUCommandBufferImpl::execute: kEndComputePass" << std::endl;
        break;
      case GPUCommand::kEndOcclusionQuery:
        std::cout << "GPUCommandBufferImpl::execute: kEndOcclusionQuery" << std::endl;
        break;
      case GPUCommand::kEndRenderPass:
      {
        GPUEndRenderPassCommand *cmd = commands_.nextCommand<GPUEndRenderPassCommand>();
        assert(cmd != nullptr && "GPUEndRenderPassCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kEndRenderPass" << std::endl;
        break;
      }
      case GPUCommand::kExecuteBundles:
        std::cout << "GPUCommandBufferImpl::execute: kExecuteBundles" << std::endl;
        break;
      case GPUCommand::kInsertDebugMarker:
      {
        GPUInsertDebugMarkerCommand *cmd = commands_.nextCommand<GPUInsertDebugMarkerCommand>();
        assert(cmd != nullptr && "GPUInsertDebugMarkerCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kInsertDebugMarker" << std::endl;
        break;
      }
      case GPUCommand::kPixelLocalStorageBarrier:
        std::cout << "GPUCommandBufferImpl::execute: kPixelLocalStorageBarrier" << std::endl;
        break;
      case GPUCommand::kPopDebugGroup:
        std::cout << "GPUCommandBufferImpl::execute: kPopDebugGroup" << std::endl;
        break;
      case GPUCommand::kPushDebugGroup:
        std::cout << "GPUCommandBufferImpl::execute: kPushDebugGroup" << std::endl;
        break;
      case GPUCommand::kResolveQuerySet:
        std::cout << "GPUCommandBufferImpl::execute: kResolveQuerySet" << std::endl;
        break;
      case GPUCommand::kSetComputePipeline:
        std::cout << "GPUCommandBufferImpl::execute: kSetComputePipeline" << std::endl;
        break;
      case GPUCommand::kSetRenderPipeline:
      {
        GPUSetRenderPipelineCommand *cmd = commands_.nextCommand<GPUSetRenderPipelineCommand>();
        assert(cmd != nullptr && "GPUSetRenderPipelineCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetRenderPipeline ("
                  << "pipelineId=" << cmd->pipelineId << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kSetStencilReference:
      {
        GPUSetStencilReferenceCommand *cmd = commands_.nextCommand<GPUSetStencilReferenceCommand>();
        assert(cmd != nullptr && "GPUSetStencilReferenceCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetStencilReference ("
                  << "reference=" << cmd->reference << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kSetViewport:
      {
        GPUSetViewportCommand *cmd = commands_.nextCommand<GPUSetViewportCommand>();
        assert(cmd != nullptr && "GPUSetViewportCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetViewport ("
                  << "x=" << cmd->x << ", "
                  << "y=" << cmd->y << ", "
                  << "width=" << cmd->width << ", "
                  << "height=" << cmd->height << ", "
                  << "minDepth=" << cmd->minDepth << ", "
                  << "maxDepth=" << cmd->maxDepth << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kSetScissorRect:
      {
        GPUSetScissorCommand *cmd = commands_.nextCommand<GPUSetScissorCommand>();
        assert(cmd != nullptr && "GPUSetScissorCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetScissorRect ("
                  << "x=" << cmd->x << ", "
                  << "y=" << cmd->y << ", "
                  << "width=" << cmd->width << ", "
                  << "height=" << cmd->height << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kSetBlendConstant:
      {
        GPUSetBlendConstantCommand *cmd = commands_.nextCommand<GPUSetBlendConstantCommand>();
        assert(cmd != nullptr && "GPUSetBlendConstantCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetBlendConstant ("
                  << "r=" << cmd->r << ", "
                  << "g=" << cmd->g << ", "
                  << "b=" << cmd->b << ", "
                  << "a=" << cmd->a << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kSetBindGroup:
      {
        GPUSetBindGroupCommand *cmd = commands_.nextCommand<GPUSetBindGroupCommand>();
        assert(cmd != nullptr && "GPUSetBindGroupCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetBindGroup ("
                  << "index=" << cmd->index << ", "
                  << "bindGroupId=" << cmd->bindGroupId << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kSetImmediateData:
        std::cout << "GPUCommandBufferImpl::execute: kSetImmediateData" << std::endl;
        break;
      case GPUCommand::kSetIndexBuffer:
      {
        GPUSetIndexBufferCommand *cmd = commands_.nextCommand<GPUSetIndexBufferCommand>();
        assert(cmd != nullptr && "GPUSetIndexBufferCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetIndexBuffer ("
                  << "bufferId=" << cmd->bufferId << ", "
                  << "offset=" << cmd->offset << ", "
                  << "size=" << cmd->size << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kSetVertexBuffer:
      {
        GPUSetVertexBufferCommand *cmd = commands_.nextCommand<GPUSetVertexBufferCommand>();
        assert(cmd != nullptr && "GPUSetVertexBufferCommand is null");
        std::cout << "GPUCommandBufferImpl::execute: kSetVertexBuffer ("
                  << "slot=" << cmd->slot << ", "
                  << "bufferId=" << cmd->bufferId << ", "
                  << "offset=" << cmd->offset << ", "
                  << "size=" << cmd->size << ")"
                  << std::endl;
        break;
      }
      case GPUCommand::kWriteBuffer:
        std::cout << "GPUCommandBufferImpl::execute: kWriteBuffer" << std::endl;
        break;
      case GPUCommand::kWriteTimestamp:
        std::cout << "GPUCommandBufferImpl::execute: kWriteTimestamp" << std::endl;
        break;

        // FIXME(yorkie): avoid default case, CommandIterator requires command type/class to be known
        // at compile time.
      }
    }
  }

  void GPUCommandBufferImpl::onDraw(const commandbuffers::GPUDrawCommand &command)
  {
    if (command.instanceCount > 1)
      glDrawArraysInstanced(GL_TRIANGLES, command.firstVertex, command.vertexCount, command.instanceCount);
    else
      glDrawArrays(GL_TRIANGLES, command.firstVertex, command.vertexCount);
  }

  void GPUCommandBufferImpl::onDraw(const commandbuffers::GPUDrawIndexedCommand &command)
  {
    if (command.instanceCount > 1)
      glDrawElementsInstanced(
        GL_TRIANGLES,
        command.indexCount,
        GL_UNSIGNED_BYTE,
        nullptr,
        command.instanceCount);
    else
      glDrawElements(
        GL_TRIANGLES,
        command.indexCount,
        GL_UNSIGNED_BYTE,
        nullptr);
  }

  void GPUCommandBufferImpl::setViewport(const commandbuffers::GPUSetViewportCommand &command)
  {
    glViewport(command.x, command.y, command.width, command.height);
  }

  void GPUCommandBufferImpl::setScissorRect(const commandbuffers::GPUSetScissorCommand &command)
  {
    glScissor(command.x, command.y, command.width, command.height);
  }
}
