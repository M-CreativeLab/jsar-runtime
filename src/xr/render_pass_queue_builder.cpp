#include "render_pass_queue_builder.hpp"
#include <algorithm>
#include "debug.hpp"

namespace xr
{
  using namespace std;
  using namespace commandbuffers;

  XRRenderPassQueueBuilder::XRRenderPassQueueBuilder()
  {
  }

  XRRenderPassQueueBuilder::~XRRenderPassQueueBuilder()
  {
    // Clear without deleting since we don't own the requests
    lock_guard<mutex> lock(m_Mutex);
    m_PendingRequests.clear();
  }

  void XRRenderPassQueueBuilder::addCommandBufferRequest(TrCommandBufferBase *request)
  {
    if (request == nullptr)
    {
      return;
    }

    lock_guard<mutex> lock(m_Mutex);
    m_PendingRequests.push_back(request);
  }

  vector<GPURenderPassEncoder> XRRenderPassQueueBuilder::buildRenderPassQueue()
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

  void XRRenderPassQueueBuilder::clear()
  {
    lock_guard<mutex> lock(m_Mutex);

    // Clear the pending requests list without deleting the requests
    // since they are owned by the StereoRenderingFrame
    m_PendingRequests.clear();
  }

  size_t XRRenderPassQueueBuilder::getRequestCount() const
  {
    lock_guard<mutex> lock(m_Mutex);
    return m_PendingRequests.size();
  }

  bool XRRenderPassQueueBuilder::hasRequests() const
  {
    lock_guard<mutex> lock(m_Mutex);
    return !m_PendingRequests.empty();
  }

  GPURenderPassEncoder XRRenderPassQueueBuilder::createRenderPassEncoderFromRequest(TrCommandBufferBase *request)
  {
    // Create a render pass encoder with a label based on the request type
    string label = "RenderPass_" + commandTypeToStr(request->type);

    // Create the encoder - this is a simple conversion for now
    // In a full implementation, this would analyze the request and create
    // appropriate render pass configuration
    GPURenderPassEncoder encoder(label);

    // TODO: Process the specific command buffer request and configure the encoder
    // This could involve setting up render targets, viewport, scissors, etc.
    // based on the content of the request

    return encoder;
  }
}