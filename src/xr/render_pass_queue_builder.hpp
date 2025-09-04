#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include "common/command_buffers/base.hpp"
#include "common/command_buffers/gpu/gpu_renderpass_encoder.hpp"

namespace xr
{
  /**
   * XRRenderPassQueueBuilder is responsible for collecting TrCommandBufferRequest objects
   * and building a queue/vector of GPURenderPassEncoder objects.
   * 
   * This class provides:
   * - Collection of TrCommandBufferRequest objects from multiple sources
   * - Generation and maintenance of an ordered queue/vector of GPURenderPassEncoder
   * - Thread-safe operations for concurrent access
   * - Efficient queue updates and resource management
   */
  class XRRenderPassQueueBuilder
  {
  public:
    XRRenderPassQueueBuilder();
    ~XRRenderPassQueueBuilder();

  public:
    /**
     * Add a command buffer request to be processed into render pass encoders.
     * Note: The builder does not take ownership of the request - it only
     * references it for processing.
     * 
     * @param request The command buffer request to add.
     */
    void addCommandBufferRequest(commandbuffers::TrCommandBufferBase *request);

    /**
     * Build and return the queue of GPU render pass encoders.
     * This method processes all accumulated command buffer requests.
     * 
     * @returns Vector of GPU render pass encoders ready for rendering.
     */
    std::vector<commandbuffers::GPURenderPassEncoder> buildRenderPassQueue();

    /**
     * Clear all accumulated command buffer requests and reset the builder.
     * Note: This does not delete the requests since the builder does not own them.
     */
    void clear();

    /**
     * Get the current count of accumulated command buffer requests.
     * 
     * @returns Number of command buffer requests waiting to be processed.
     */
    size_t getRequestCount() const;

    /**
     * Check if the builder has any accumulated requests.
     * 
     * @returns True if there are pending requests, false otherwise.
     */
    bool hasRequests() const;

  private:
    /**
     * Convert a single command buffer request to a GPU render pass encoder.
     * 
     * @param request The command buffer request to convert.
     * @returns GPU render pass encoder created from the request.
     */
    commandbuffers::GPURenderPassEncoder createRenderPassEncoderFromRequest(commandbuffers::TrCommandBufferBase *request);

  private:
    mutable std::mutex m_Mutex;
    std::vector<commandbuffers::TrCommandBufferBase *> m_PendingRequests;
  };
}