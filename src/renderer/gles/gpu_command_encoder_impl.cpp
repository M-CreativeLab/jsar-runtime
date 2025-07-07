#include "./common.hpp"
#include "./gpu_command_encoder_impl.hpp"
#include "./gpu_command_buffer_impl.hpp"
#include "./gpu_renderpass_encoder_impl.hpp"

namespace gles
{
  using namespace std;
  using namespace commandbuffers;

  GPURenderPassEncoder GPUCommandEncoderImpl::beginRenderPass(GPURenderPassDescriptor &)
  {
    assert(false && "beginRenderPass not implemented in gles::GPUCommandEncoderImpl");
  }

  std::unique_ptr<GPUCommandBuffer> GPUCommandEncoderImpl::finish(std::optional<std::string> label) const
  {
    if (current_pass_encoder_ == nullptr)
      return nullptr;

    if (current_pass_encoder_->isRenderPassEncoder())
    {
      const auto &renderpass_encoder = dynamic_cast<const GPURenderPassEncoderImpl &>(*current_pass_encoder_);
      return unique_ptr<GPUCommandBuffer>(new GPUCommandBufferImpl(label, renderpass_encoder));
    }
    else
    {
      return nullptr;
    }
  }

  GPURenderPassEncoder &GPUCommandEncoderImpl::getOrStartRecordingRenderPass()
  {
    if (current_pass_encoder_ == nullptr || !current_pass_encoder_->isRenderPassEncoder())
    {
      unique_ptr<GPURenderPassEncoderImpl> renderpass_encoder = make_unique<GPURenderPassEncoderImpl>();
      renderpass_encoder->begin();
      current_pass_encoder_ = move(renderpass_encoder);
    }

    auto renderpass_encoder = dynamic_pointer_cast<GPURenderPassEncoderImpl>(current_pass_encoder_);
    assert(renderpass_encoder != nullptr && "Current pass encoder must be a GPURenderPassEncoderImpl");
    return *renderpass_encoder;
  }

  bool GPUCommandEncoderImpl::isRenderPassWith(GLuint target_framebuffer) const
  {
    if (current_pass_encoder_ == nullptr || !current_pass_encoder_->isRenderPassEncoder())
      return false;

    const auto &renderpass_encoder = dynamic_cast<const GPURenderPassEncoderImpl &>(*current_pass_encoder_);
    return renderpass_encoder.targetFramebuffer() == target_framebuffer;
  }
}
