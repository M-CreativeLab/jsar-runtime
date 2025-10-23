#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <common/command_buffers/gpu/gpu_command_buffer.hpp>

namespace client_graphics
{
  /**
   * The `WebGPUCommandBuffer` class represents a recorded sequence of GPU commands.
   * This is the client-side implementation that holds recorded commands for later submission.
   */
  class WebGPUCommandBuffer
  {
  public:
    WebGPUCommandBuffer(std::optional<std::string> label = std::nullopt);
    ~WebGPUCommandBuffer() = default;

    // Non-copyable but movable
    WebGPUCommandBuffer(const WebGPUCommandBuffer &) = delete;
    WebGPUCommandBuffer &operator=(const WebGPUCommandBuffer &) = delete;
    WebGPUCommandBuffer(WebGPUCommandBuffer &&) = default;
    WebGPUCommandBuffer &operator=(WebGPUCommandBuffer &&) = default;

  public:
    const std::string &label() const
    {
      return label_;
    }
    bool isEmpty() const
    {
      return commands_.empty();
    }
    size_t commandCount() const
    {
      return commands_.size();
    }

    /**
     * Execute the recorded commands. In the client-side implementation,
     * this would typically transmit the commands to the server.
     * For now, this is a placeholder for the command recording pattern.
     */
    void execute() const;

  private:
    friend class WebGPUCommandEncoder;
    friend class WebGPURenderPassEncoder;

    std::string label_;
    std::vector<std::shared_ptr<commandbuffers::GPUCommand>> commands_;

    /**
     * Add a command to the command buffer.
     * This follows the same pattern as the existing GPU command buffer implementation.
     */
    template <typename T, typename... Args>
    void addCommand(Args &&...args)
    {
      auto command = std::make_shared<T>(std::forward<Args>(args)...);
      commands_.push_back(command);
    }
  };
}