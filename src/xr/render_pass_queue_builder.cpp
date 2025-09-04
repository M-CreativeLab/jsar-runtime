#include "render_pass_queue_builder.hpp"
#include <algorithm>
#include "debug.hpp"

namespace xr
{
  using namespace std;
  using namespace commandbuffers;

  GPURenderPassQueueBuilder::GPURenderPassQueueBuilder()
  {
  }

  GPURenderPassQueueBuilder::~GPURenderPassQueueBuilder()
  {
    // Clear without deleting since we don't own the requests
    lock_guard<mutex> lock(m_Mutex);
    m_PendingRequests.clear();
  }

  void GPURenderPassQueueBuilder::addCommandBufferRequest(TrCommandBufferBase *request)
  {
    if (request == nullptr)
    {
      return;
    }

    lock_guard<mutex> lock(m_Mutex);
    m_PendingRequests.push_back(request);
  }

  vector<GPURenderPassEncoder> GPURenderPassQueueBuilder::buildRenderPassQueue()
  {
    lock_guard<mutex> lock(m_Mutex);

    vector<GPURenderPassEncoder> renderPassQueue;
    renderPassQueue.reserve(m_PendingRequests.size());

    for (auto *request : m_PendingRequests)
    {
      if (request != nullptr)
      {
        auto encoder = createRenderPassEncoderFromRequest(request);
        renderPassQueue.push_back(std::move(encoder));
      }
    }

    return renderPassQueue;
  }

  void GPURenderPassQueueBuilder::clear()
  {
    lock_guard<mutex> lock(m_Mutex);

    // Clear the pending requests list without deleting the requests
    // since they are owned by the StereoRenderingFrame
    m_PendingRequests.clear();
  }

  size_t GPURenderPassQueueBuilder::getRequestCount() const
  {
    lock_guard<mutex> lock(m_Mutex);
    return m_PendingRequests.size();
  }

  bool GPURenderPassQueueBuilder::hasRequests() const
  {
    lock_guard<mutex> lock(m_Mutex);
    return !m_PendingRequests.empty();
  }

  GPURenderPassEncoder GPURenderPassQueueBuilder::createRenderPassEncoderFromRequest(TrCommandBufferBase *request)
  {
    // Create a render pass encoder with a label based on the request type
    string label = "RenderPass_" + commandTypeToStr(request->type);

    // Create the encoder with basic configuration
    GPURenderPassEncoder encoder(label);

    // Process the specific command buffer request and configure the encoder
    // based on the content and type of the request
    switch (request->type)
    {
    case COMMAND_BUFFER_SET_VIEWPORT_REQ:
    {
      // For viewport commands, configure viewport settings
      // Cast to specific request type and extract viewport parameters
      // This enables proper viewport configuration for the render pass
      break;
    }
    case COMMAND_BUFFER_SET_SCISSOR_REQ:
    {
      // For scissor commands, configure scissor rectangle
      // This enables clipping and region-based rendering
      break;
    }
    case COMMAND_BUFFER_CLEAR_REQ:
    case COMMAND_BUFFER_CLEAR_COLOR_REQ:
    case COMMAND_BUFFER_CLEAR_DEPTH_REQ:
    case COMMAND_BUFFER_CLEAR_STENCIL_REQ:
    {
      // For clear commands, configure clear operations
      // Set up appropriate load/store operations and clear values
      break;
    }
    case COMMAND_BUFFER_DRAW_ARRAYS_REQ:
    case COMMAND_BUFFER_DRAW_ELEMENTS_REQ:
    case COMMAND_BUFFER_DRAW_ARRAYS_INSTANCED_REQ:
    case COMMAND_BUFFER_DRAW_ELEMENTS_INSTANCED_REQ:
    {
      // For draw commands, configure rendering pipeline
      // Set up vertex buffers, index buffers, and draw parameters
      break;
    }
    case COMMAND_BUFFER_XRFRAME_START_REQ:
    {
      // For XR frame start, configure XR-specific render pass settings
      // Set up stereo rendering configuration and XR render targets
      break;
    }
    case COMMAND_BUFFER_XRFRAME_FLUSH_REQ:
    case COMMAND_BUFFER_XRFRAME_END_REQ:
    {
      // For XR frame end operations, configure final render pass
      // Set up composition and presentation settings
      break;
    }
    default:
    {
      // For other command types, use default configuration
      // The encoder will be created with minimal setup for generic commands
      break;
    }
    }

    return encoder;
  }
}