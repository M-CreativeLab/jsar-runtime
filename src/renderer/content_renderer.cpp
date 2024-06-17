#include <chrono>
#include "content_renderer.hpp"
#include "runtime/content.hpp"
#include "runtime/constellation.hpp"
#include "xr/device.hpp"
#include "xr/session.hpp"

namespace renderer
{
  TrContentRenderer::TrContentRenderer(TrContentRuntime *content, TrConstellation *constellation)
      : content(content),
        constellation(constellation),
        xrDevice(constellation->getXrDevice()),
        currentBaseXRFrameReq(new xr::TrXRFrameRequest())
  {
    assert(xrDevice != nullptr);
    glContext = new OpenGLAppContextStorage("content_renderer#" + std::to_string(content->pid));
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

  pid_t TrContentRenderer::getContentPid()
  {
    return content->pid;
  }

  void TrContentRenderer::onHostFrame()
  {
    // TODO: implement frame rate control
    dispatchAnimationFrameRequest();
    if (xrDevice->enabled())
    {
      bool shouldDispatchXRFrame = false;
      xr::StereoRenderingFrame *stereoRenderingFrame = nullptr;
      // auto renderer = constellation->getRenderer();
      // auto rhi = renderer->getApi();

      if (xrDevice->getStereoRenderingMode() == xr::TrStereoRenderingMode::MultiPass)
      {
        auto viewIndex = xrDevice->getActiveEyeId();
        stereoRenderingFrame = xrDevice->createOrGetStereoRenderingFrame();
        if (stereoRenderingFrame != nullptr)
        {
          xr::TrXRView view(viewIndex);
          auto viewport = xrDevice->getViewport(viewIndex);
          view.setViewport(viewport.width, viewport.height, viewport.x, viewport.y);
          view.setViewMatrix(xrDevice->getViewerStereoViewMatrix(viewIndex));
          view.setProjectionMatrix(xrDevice->getViewerStereoProjectionMatrix(viewIndex));

          if (viewIndex == 0) // Reset the `currentBaseXRFrameReq` when viewIndex is 0(left)
          {
            currentBaseXRFrameReq->reset();
            // Set `currentBaseXRFrameReq` with the related data.
            currentBaseXRFrameReq->stereoId = stereoRenderingFrame->getId();
            currentBaseXRFrameReq->setViewerBaseMatrix(xrDevice->getViewerTransform());
          }
          currentBaseXRFrameReq->views[viewIndex] = view;
          if (viewIndex == 1) // Dispatch XR frame request when viewIndex is 1(right)
            shouldDispatchXRFrame = true;
        }
        else
        {
          // TODO: OOM handling?
        }
      }
      else
      {
        // TODO: support other stereo rendering modes such as SinglePass
      }

      if (shouldDispatchXRFrame && stereoRenderingFrame != nullptr)
      {
        xrDevice->iterateSessionsByContentPid(content->pid, [this](xr::TrXRSession *session)
                                              { this->dispatchXRFrameRequest(session); });
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

  inline void TrContentRenderer::dispatchXRFrameRequest(xr::TrXRSession *session)
  {
    auto req = currentBaseXRFrameReq->clone();
    req.sessionId = session->id;
    req.setLocalBaseMatrix(session->getLocalBaseMatrix());
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
