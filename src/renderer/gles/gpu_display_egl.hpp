#pragma once

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace gles
{
  class DisplayEGL
  {
  public:
    static Ref<DisplayEGL> CreateFromCurrent();

    explicit DisplayEGL(commandbuffers::GPUBackendType);

  private:
    void initializeFromCurrent();
  };
}
