#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace client_graphics
{
  /**
   * Represents a WebGPU render pipeline.
   * This is a placeholder implementation for the client-side render pipeline interface.
   */
  class WebGPURenderPipeline
  {
  public:
    WebGPURenderPipeline(std::string label = "WebGPURenderPipeline")
        : label_(std::move(label))
        , id_(commandbuffers::GPUHandle("").id) // Generate unique ID
    {
    }

    const std::string &label() const
    {
      return label_;
    }
    commandbuffers::GPUIdentifier id() const
    {
      return id_;
    }

  private:
    std::string label_;
    commandbuffers::GPUIdentifier id_;
  };
}