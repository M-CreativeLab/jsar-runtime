#include "./webgpu_queue.hpp"
#include "./webgpu_command_buffer.hpp"

#include <iostream>

namespace client_graphics
{
  using namespace std;

  // WebGPUQueue implementation
  WebGPUQueue::WebGPUQueue(std::optional<std::string> label)
      : label_(label.value_or("WebGPUQueue"))
  {
  }

  void WebGPUQueue::submit(const std::vector<std::unique_ptr<WebGPUCommandBuffer>> &commandBuffers)
  {
    // In the client-side implementation, we record the submission
    // In a full implementation, this would transmit commands to the server
    for (const auto &commandBuffer : commandBuffers)
    {
      if (commandBuffer && !commandBuffer->isEmpty())
      {
        // Execute the command buffer (placeholder)
        commandBuffer->execute();
      }
    }
  }
}