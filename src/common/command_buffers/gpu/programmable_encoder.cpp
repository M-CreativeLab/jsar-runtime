#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/programmable_encoder.hpp>

namespace commandbuffers::gpu
{
  using namespace std;

  ProgrammableEncoder::ProgrammableEncoder(Ref<GPUDeviceBase> device,
                                           string_view label,
                                           EncodingContext *encoding_context)
      : GPUHandle(device, label)
      , encoding_context_(encoding_context)
      , validation_enabled_(device->isValidationEnabled())
  {
  }

  ProgrammableEncoder::ProgrammableEncoder(Ref<GPUDeviceBase> device,
                                           EncodingContext *encodingContext,
                                           ErrorTag errorTag,
                                           string_view label)
      : GPUHandle(device, errorTag, label)
      , encoding_context_(encodingContext)
      , validation_enabled_(device->isValidationEnabled())
  {
  }

  void ProgrammableEncoder::insertDebugMarker(string_view group_label)
  {
  }

  void ProgrammableEncoder::popDebugMarker()
  {
  }

  void ProgrammableEncoder::pushDebugMarker(string_view group_label)
  {
  }

  void ProgrammableEncoder::setImmediateData(uint32_t offset, const void *data, size_t size)
  {
  }

  bool ProgrammableEncoder::isValidationEnabled() const
  {
    return validation_enabled_;
  }

  MaybeError ProgrammableEncoder::validateProgrammableEncoderEnd() const
  {
    return {};
  }
}