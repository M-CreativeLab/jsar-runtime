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
    throw runtime_error("beginRenderPass not implemented in gles::GPUCommandEncoderImpl");
  }

  GPUCommandBuffer GPUCommandEncoderImpl::finish(optional<string> label)
  {
    return GPUCommandBufferImpl();
  }

  GPURenderPassEncoder &GPUCommandEncoderImpl::getOrStartRecordingRenderPass()
  {
    if (recording_renderpass_encoder_ != nullptr)
    {
      return *recording_renderpass_encoder_;
    }
    else
    {
      recording_renderpass_encoder_ = make_unique<GPURenderPassEncoderImpl>();
      return *recording_renderpass_encoder_;
    }
  }
}
