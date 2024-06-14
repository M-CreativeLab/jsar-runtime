#include <chrono>
#include "content_renderer.hpp"
#include "runtime/content.hpp"
#include "runtime/constellation.hpp"

namespace renderer
{
  TrContentRenderer::TrContentRenderer(TrContentRuntime *content, TrConstellation *constellation)
      : content(content),
        constellation(constellation),
        xrDevice(constellation->getXrDevice())
  {
    assert(xrDevice != nullptr);
    glContext = new OpenGLAppContextStorage("content_renderer");
  }

  TrContentRenderer::~TrContentRenderer()
  {
    content = nullptr;
    xrDevice = nullptr;
    if (glContext == nullptr)
    {
      delete glContext;
      glContext = nullptr;
    }
  }

  void TrContentRenderer::onCommandBuffersExecuting()
  {
    content->onCommandBuffersExecuting();
  }

  void TrContentRenderer::onCommandBuffersExecuted()
  {
    content->onCommandBuffersExecuted();
  }

  bool TrContentRenderer::sendCommandBufferResponse(TrCommandBufferResponse &res)
  {
    return content->sendCommandBufferResponse(res);
  }

  OpenGLAppContextStorage *TrContentRenderer::getOpenGLContext()
  {
    return glContext;
  }

  TrContentRuntime *TrContentRenderer::getContent()
  {
    return content;
  }

  void TrContentRenderer::onHostFrame()
  {
    // TODO: implement frame rate control
    dispatchAnimationFrameRequest();
    if (xrDevice->enabled())
    {
      bool shouldDispatchXRFrame = false;
      if (xrDevice->getStereoRenderingMode() == xr::TrStereoRenderingMode::MultiPass)
      {
        if (xrDevice->getActiveEyeId() == 1) // Only dispatch the frame for the right eye
          shouldDispatchXRFrame = true;
      }
      else
      {
        // TODO: support other stereo rendering modes such as SinglePass
      }
      if (shouldDispatchXRFrame)
      {
        // TODO: create a new frame request message from `xrDevice`.
        TrXRFrameRequest req;
        dispatchFrameRequest(req);
      }
    }

    onStartFrame();
    {
      executeCommandBuffers();
      if (xrDevice->enabled())
      {
        // set framebuffer?
        // TODO: execute XR frame command buffers
      }
    }
    onEndFrame();
  }

  void TrContentRenderer::onStartFrame()
  {
    glContext->Restore();
    glContext->Print();
  }

  void TrContentRenderer::onEndFrame()
  {
  }

  inline void TrContentRenderer::dispatchAnimationFrameRequest()
  {
    TrAnimationFrameRequest req;
    dispatchFrameRequest(req);
  }

  void TrContentRenderer::executeCommandBuffers()
  {
    if (content == nullptr) // FIXME: just skip executing command buffers if content is null, when content process is crashed.
      return;
    vector<commandbuffers::TrCommandBufferBase *> commandBufferRequests;
    {
      lock_guard<mutex> lock(content->commandBufferRequestsMutex);
      commandBufferRequests = content->commandBufferRequests;
      content->commandBufferRequests.clear();
    }
    constellation->getRenderer()->executeCommandBuffers(commandBufferRequests, this);
  }

  void TrContentRenderer::resetFrameRequestChanSenderWith(ipc::TrOneShotClient<TrFrameRequestMessage> *client)
  {
    if (client == nullptr)
      return;
    if (frameRequestChanSender != nullptr)
      delete frameRequestChanSender;
    frameRequestChanSender = new frame_request::TrFrameRequestSender(client);
  }
}
