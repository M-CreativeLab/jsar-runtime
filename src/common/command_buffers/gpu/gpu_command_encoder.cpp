#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_commands.hpp>
#include <common/command_buffers/gpu/command_allocator.hpp>

using namespace std;

namespace commandbuffers
{
  Ref<GPUCommandEncoder> GPUCommandEncoder::Create(Ref<GPUDeviceBase> device,
                                                   const GPUCommandEncoderDescriptor &descriptor)
  {
    return AcquireRef(new GPUCommandEncoder(device, descriptor));
  }

  Ref<GPUCommandEncoder> GPUCommandEncoder::MakeError(Ref<GPUDeviceBase> device, string_view label)
  {
    return AcquireRef(new GPUCommandEncoder(device, GPUHandle::kError, label));
  }

  GPUCommandEncoder::GPUCommandEncoder(Ref<GPUDeviceBase> device, const GPUCommandEncoderDescriptor &descriptor)
      : GPUHandle(device, descriptor.label)
      , encoding_context_(device, this)
      , usage_validation_mode_(GPUUsageValidationMode::kDefault)
  {
  }

  GPUCommandEncoder::GPUCommandEncoder(Ref<GPUDeviceBase> device,
                                       GPUHandle::ErrorTag tag,
                                       string_view label)
      : GPUHandle(device, tag, label)
      , encoding_context_(device, this)
      , usage_validation_mode_(GPUUsageValidationMode::kDefault)
  {
  }

  GPURenderPassEncoder GPUCommandEncoder::beginRenderPass(GPURenderPassDescriptor &)
  {
    throw runtime_error("beginRenderPass is not implemented");
  }

  void GPUCommandEncoder::clearBuffer()
  {
    throw runtime_error("clearBuffer is not implemented");
  }

  void GPUCommandEncoder::copyBufferToBuffer()
  {
    throw runtime_error("copyBufferToBuffer is not implemented");
  }

  void GPUCommandEncoder::copyBufferToTexture()
  {
    throw runtime_error("copyBufferToTexture is not implemented");
  }

  void GPUCommandEncoder::copyTextureToBuffer()
  {
    throw runtime_error("copyTextureToBuffer is not implemented");
  }

  void GPUCommandEncoder::copyTextureToTexture()
  {
    throw runtime_error("copyTextureToTexture is not implemented");
  }

  unique_ptr<GPUCommandBufferBase> GPUCommandEncoder::finish(optional<string> label) const
  {
    return nullptr;
  }

  void GPUCommandEncoder::insertDebugMarker(std::string_view marker)
  {
    encoding_context_.tryEncode(
      this,
      [&](gpu::CommandAllocator *allocator) -> gpu::MaybeError
      {
        GPUInsertDebugMarkerCommand *cmd = allocator->allocate<GPUInsertDebugMarkerCommand>(GPUCommand::kInsertDebugMarker);
        return {};
      });
  }

  void GPUCommandEncoder::pushDebugGroup(std::string_view group)
  {
    throw runtime_error("pushDebugGroup is not implemented");
  }

  void GPUCommandEncoder::popDebugGroup()
  {
    throw runtime_error("popDebugGroup is not implemented");
  }

  void GPUCommandEncoder::resolveQuerySet()
  {
    throw runtime_error("resolveQuerySet is not implemented");
  }

  void GPUCommandEncoder::writeTimestamp()
  {
    throw runtime_error("writeTimestamp is not implemented");
  }
}
