#pragma once

#include <string>
#include <memory>

#include "./gpu_texture.hpp"

namespace commandbuffers
{
  class GPUTextureViewBase : public GPUHandle
  {
  public:
    const GPUTexture &textureRef() const
    {
      auto texture = texture_.lock();
      if (!texture)
        throw std::runtime_error("GPUTextureView: Texture has been destroyed.");
      return *texture;
    }

  private:
    std::weak_ptr<GPUTexture> texture_;
  };
}
