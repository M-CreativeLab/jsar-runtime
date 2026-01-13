#include "./common.hpp"
#include "./framebuffer.hpp"
#include "./gpu_command_buffer_impl.hpp"

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
      default:
        std::cerr << "GPUCommandBufferImpl::execute: unknown command type"
                  << static_cast<int>(type)
                  << std::endl;
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
