#pragma once

#include <string>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace client_graphics
{
  /**
   * Represents a WebGPU bind group.
   * This is a placeholder implementation for the client-side bind group interface.
   */
  class WebGPUBindGroup
  {
  public:
    WebGPUBindGroup(std::string label = "WebGPUBindGroup")
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