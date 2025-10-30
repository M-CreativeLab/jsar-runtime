#include "./common.hpp"
#include "./framebuffer.hpp"
#include "./gpu_command_buffer_impl.hpp"

namespace gles
{
  using namespace std;
  using namespace commandbuffers;

  GPUCommandBufferImpl::GPUCommandBufferImpl(optional<string> label)
      : GPUCommandBufferBase(move(label))
      , framebuffer_(0)
  {
  }

  GPUCommandBufferImpl::GPUCommandBufferImpl(optional<string> label, const GPURenderPassEncoderImpl &renderpass_encoder)
      : GPUCommandBufferBase(move(label), renderpass_encoder.commandBuffer())
      , framebuffer_(renderpass_encoder.framebuffer_)
  {
    assert(framebuffer_ > 0 && "Framebuffer must be bound before creating GPUCommandBufferImpl");
  }

  void GPUCommandBufferImpl::execute()
  {
    GLFramebuffer framebuffer(framebuffer_, false);
    GLFramebufferScope framebuffer_scope(GL_FRAMEBUFFER, framebuffer);

    // TODO(yorkie): execute the commands in the command buffer.
    for (const auto &command : commands_)
    {
      switch (command->type)
      {
      case GPUCommand::kDraw:
        onDraw(*dynamic_pointer_cast<GPUDrawCommand>(command));
        break;
      case GPUCommand::kDrawIndexed:
        onDraw(*dynamic_pointer_cast<GPUDrawIndexedCommand>(command));
        break;
      case GPUCommand::kSetViewport:
        setViewport(*dynamic_pointer_cast<GPUSetViewportCommand>(command));
        break;
      case GPUCommand::kSetScissorRect:
        setScissorRect(*dynamic_pointer_cast<GPUSetScissorCommand>(command));
        break;
      default:
        break;
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
