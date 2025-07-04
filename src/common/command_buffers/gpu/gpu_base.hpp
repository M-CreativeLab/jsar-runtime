#pragma once

#include <string>

namespace commandbuffers
{
  enum class GPUShaderStage
  {
    kVertex,
    kFragment,
    kCompute,
  };

  enum class GPUBufferType
  {
    kReadOnlyStorage,
    kStorage,
    kUniform,
  };

  enum class GPUStorageAccess
  {
    kReadOnly,
    kReadWrite,
    kWriteOnly,
  };

  enum class GPUSamplerType
  {
    kComparison,
    kFiltering,
    kNonFiltering,
  };

  class GPUHandle
  {
  public:
    GPUHandle(std::string label = "")
        : label(std::move(label))
    {
    }

  public:
    const std::string label = "";
  };
}
