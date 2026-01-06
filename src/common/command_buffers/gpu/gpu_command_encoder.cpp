#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_commands.hpp>
#include <common/command_buffers/gpu/command_allocator.hpp>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>

using namespace std;

namespace commandbuffers
{
  class RenderPassValidationState final : NonMovable
  {
  public:
    RenderPassValidationState() = default;
    ~RenderPassValidationState() = default;


    uint32_t getRenderWidth() const
    {
      return render_width_;
    }
    uint32_t getRenderHeight() const
    {
      return render_height_;
    }
    uint32_t getSampleCount() const
    {
      return sample_count_;
    }
    uint32_t getImplicitSampleCount() const
    {
      return implicit_sample_count_;
    }

  private:
    uint32_t render_width_ = 0;
    uint32_t render_height_ = 0;
    uint32_t sample_count_ = 0;
    // The implicit multisample count used by MSAA render to single sampled.
    uint32_t implicit_sample_count_ = 0;
  };

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

  Ref<GPURenderPassEncoder> GPUCommandEncoder::beginRenderPass(GPURenderPassDescriptor &descriptor)
  {
    Ref<GPUDeviceBase> device_ = device();
    bool depthReadOnly = false;
    bool stencilReadOnly = false;

    RenderPassValidationState validationState;

    auto MakeError = [&]()
    {
      return GPURenderPassEncoder::MakeError(this,
                                             &encoding_context_,
                                             descriptor.label.value_or(""));
    };

    bool success = encoding_context_.tryEncode(
      this,
      [&](gpu::CommandAllocator *allocator) -> gpu::MaybeError
      {
        GPUBeginRenderPassCommand *cmd = allocator->allocate<GPUBeginRenderPassCommand>(GPUCommand::kBeginRenderPass);
        return {};
      });

    if (success)
    {
      Ref<GPURenderPassEncoder> passEncoder = GPURenderPassEncoder::Create(descriptor,
                                                                           this,
                                                                           &encoding_context_,
                                                                           validationState.getRenderWidth(),
                                                                           validationState.getRenderHeight(),
                                                                           depthReadOnly,
                                                                           stencilReadOnly);

      encoding_context_.enterPass(passEncoder.get());
      return passEncoder;
    }

    return MakeError();
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
    string effective_label = label.has_value() ? *label : getLabel();
    GPUCommandBufferDescriptor desc = { string_view(effective_label) };
    return device()->createCommandBuffer(*const_cast<GPUCommandEncoder *>(this), &desc);
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

  gpu::CommandIterator GPUCommandEncoder::acquireCommands()
  {
    return encoding_context_.acquireCommands();
  }

  gpu::CommandBufferResourceUsage GPUCommandEncoder::acquireResourceUsages() const
  {
    gpu::CommandBufferResourceUsage usages;
    return usages;
  }

  std::vector<gpu::IndirectDrawMetadata> GPUCommandEncoder::acquireIndirectDrawMetadata() const
  {
    return {};
  }
}
