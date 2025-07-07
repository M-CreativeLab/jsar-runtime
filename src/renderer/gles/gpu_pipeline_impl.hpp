#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>

namespace gles
{
  class GPURenderPipelineImpl : public commandbuffers::GPURenderPipeline
  {
  public:
    GPURenderPipelineImpl(GLuint program)
        : commandbuffers::GPURenderPipeline("GPURenderPipeline")
        , program_(program)
    {
    }

  private:
    GLuint program_;
  };
}
