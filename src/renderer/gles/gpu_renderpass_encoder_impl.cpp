#include "./common.hpp"
#include "./gpu_command_buffer_impl.hpp"
#include "./gpu_renderpass_encoder_impl.hpp"

namespace gles
{
  using namespace std;
  using namespace commandbuffers;

  GPURenderPassEncoderImpl::GPURenderPassEncoderImpl(std::string label)
      : commandbuffers::GPURenderPassEncoder(label)
      , framebuffer_(0)
  {
  }

  void GPURenderPassEncoderImpl::begin()
  {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint *>(&framebuffer_));
    assert(framebuffer_ > 0 && "Framebuffer must be bound before beginning render pass");
    command_buffer_ = make_unique<GPUCommandBufferImpl>(label);

    GPURenderPassEncoder::begin();
  }
}
