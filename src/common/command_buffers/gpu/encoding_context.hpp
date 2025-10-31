#pragma once

#include <memory>
#include <string_view>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/error_data.hpp>
#include <common/command_buffers/gpu/command_allocator.hpp>
#include <common/command_buffers/gpu/indirect_draw_metadata.hpp>
#include <common/command_buffers/gpu/pass_resource_usage.hpp>
#include <common/command_buffers/gpu/pass_resource_usage_tracker.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;
  class GPUCommandEncoder;

  namespace gpu
  {
    class EncodingContext
    {
    public:
      EncodingContext(std::shared_ptr<GPUDeviceBase> device, std::shared_ptr<const GPUHandle> initialEncoder);
      EncodingContext(std::shared_ptr<GPUDeviceBase> device, GPUHandle::ErrorTag tag);
      ~EncodingContext();

      // Marks the encoding context as destroyed so that any future encodes will fail, and all
      // encoded commands are released.
      void destroy();
      CommandIterator acquireCommands();

      // Must be called prior to encoding a BeginRenderPassCmd. Note that it's OK to call this
      // and then not actually call EnterPass+ExitRenderPass, for example if some other pass setup
      // failed validation before the BeginRenderPassCmd could be encoded.
      void willBeginRenderPass();

      // Functions to set current encoder state
      void enterPass(const GPUHandle *passEncoder);
      bool exitRenderPass(const GPUHandle *passEncoder,
                          RenderPassResourceUsageTracker usageTracker,
                          GPUCommandEncoder *commandEncoder,
                          gpu::IndirectDrawMetadata indirectDrawMetadata);
      void exitComputePass(const GPUHandle *passEncoder, ComputePassResourceUsage usages);
      bool finish();

      // Called when a pass encoder is deleted. Provides an opportunity to clean up if it's the
      // mCurrentEncoder.
      void ensurePassExited(const GPUHandle *passEncoder);

      void pushDebugGroupLabel(std::string_view groupLabel);
      void popDebugGroupLabel();

    private:
      enum class Status
      {
        kOpen,
        kFinished,
        kErrorAtCreation,
        kErrorInRecording,
        kDestroyed,
      };

      void commitCommands(CommandAllocator allocator);
      void closeWithStatus(Status status);

      GPUDeviceBase *device_;
      const GPUHandle *top_level_encoder_;
      const GPUHandle *current_encoder_;

      CommandAllocator pending_commands_;
      std::vector<CommandAllocator> allocators_;
      bool were_commands_acquired_ = false;

      // Contains pointers to strings allocated inside the command allocators.
      std::vector<std::string_view> debug_group_labels_;

      Status status_;
      std::unique_ptr<ErrorData> error_;
    };
  }
}
