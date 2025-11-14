#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/encoding_context.hpp>

using namespace std;

namespace commandbuffers::gpu
{
  EncodingContext::EncodingContext(Ref<GPUDeviceBase> device, const GPUHandle *initialEncoder)
      : device_(device.get())
      , top_level_encoder_(initialEncoder)
      , current_encoder_(initialEncoder)
      , status_(Status::kOpen)
  {
    assert(!initialEncoder->isError());
  }

  EncodingContext::EncodingContext(Ref<GPUDeviceBase> device, GPUHandle::ErrorTag tag)
      : device_(device.get())
      , top_level_encoder_(nullptr)
      , current_encoder_(nullptr)
      , status_(Status::kErrorAtCreation)
  {
  }

  EncodingContext::~EncodingContext()
  {
    destroy();
  }

  void EncodingContext::destroy()
  {
    debug_group_labels_.clear();

    if (!were_commands_acquired_)
    {
      //   mIndirectDrawMetadata.clear();
    }
    // if (!mWereCommandsAcquired)
    // {
    //   CommandIterator commands = AcquireCommands();
    //   FreeCommands(&commands);
    // }

    closeWithStatus(Status::kDestroyed);
  }

  CommandIterator EncodingContext::acquireCommands()
  {
    assert(!were_commands_acquired_);
    were_commands_acquired_ = true;

    commitCommands(std::move(pending_commands_));

    CommandIterator commands;
    commands.acquireCommandBlocks(std::move(allocators_));
    return commands;
  }

  void EncodingContext::willBeginRenderPass()
  {
    assert(current_encoder_ == top_level_encoder_);
    if (device_->isValidationEnabled() || device_->mayRequireDuplicationOfIndirectParameters())
    {
      // When validation is enabled or indirect parameters require duplication, we are going
      // to want to capture all commands encoded between and including BeginRenderPassCmd and
      // EndRenderPassCmd, and defer their sequencing util after we have a chance to insert
      // any necessary validation or duplication commands. To support this we commit any
      // current commands now, so that the impending BeginRenderPassCmd starts in a fresh
      // CommandAllocator.
      commitCommands(std::move(pending_commands_));
    }
  }

  void EncodingContext::enterPass(const GPUHandle *passEncoder)
  {
    // Assert we're at the top level.
    assert(current_encoder_ == top_level_encoder_);
    assert(passEncoder != nullptr);

    current_encoder_ = passEncoder;
  }

  void EncodingContext::exitComputePass(const GPUHandle *passEncoder,
                                        ComputePassResourceUsage usages)
  {
    assert(current_encoder_ != top_level_encoder_);
    assert(current_encoder_ == passEncoder);

    current_encoder_ = top_level_encoder_;
    // mComputePassUsages.push_back(std::move(usages));
  }

  bool EncodingContext::finish()
  {
    return false;
  }

  void EncodingContext::ensurePassExited(const GPUHandle *passEncoder)
  {
    if (current_encoder_ != top_level_encoder_ && current_encoder_ == passEncoder)
    {
      // The current pass encoder is being deleted. Implicitly end the pass with an error.
      current_encoder_ = top_level_encoder_;
      // HandleError(DAWN_VALIDATION_ERROR("Command buffer recording ended before %s was ended.",
      //                                   passEncoder));
    }
  }

  void EncodingContext::pushDebugGroupLabel(string_view groupLabel)
  {
  }

  void EncodingContext::popDebugGroupLabel()
  {
  }

  void EncodingContext::commitCommands(CommandAllocator allocator)
  {
  }

  void EncodingContext::closeWithStatus(Status status)
  {
  }
}
