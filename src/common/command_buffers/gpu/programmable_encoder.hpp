#pragma once

#include <string_view>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/encoding_context.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;

  namespace gpu
  {
    class ProgrammableEncoder : public GPUHandle
    {
    public:
      ProgrammableEncoder(Ref<GPUDeviceBase> device,
                          std::string_view label,
                          EncodingContext *encoding_context);

      void insertDebugMarker(std::string_view group_label);
      void popDebugMarker();
      void pushDebugMarker(std::string_view group_label);
      void setImmediateData(uint32_t offset, const void *data, size_t size);

    protected:
      bool isValidationEnabled() const;
      MaybeError validateProgrammableEncoderEnd() const;

      // Construct an "error" programmable pass encoder.
      ProgrammableEncoder(Ref<GPUDeviceBase> device,
                          EncodingContext *encodingContext,
                          ErrorTag errorTag,
                          std::string_view label);

      EncodingContext *encoding_context_ = nullptr;
      uint64_t debug_group_stack_size_ = 0;
      bool ended_ = false;

    private:
      const bool validation_enabled_;
    };
  }
}
