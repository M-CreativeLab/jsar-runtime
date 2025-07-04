#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>

namespace gles
{
  class GPURenderPassEncoderImpl : public commandbuffers::GPURenderPassEncoder
  {
    using commandbuffers::GPURenderPassEncoder::GPURenderPassEncoder;
  };
}
