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
        targetFrameRate(constellation->getRenderer()->clientDefaultFrameRate),
        currentBaseXRFrameReq(new xr::TrXRFrameRequest())
  {
    assert(xrDevice != nullptr);
    glContext = new OpenGLAppContextStorage("content_renderer#" + std::to_string(content->pid));
    stereoFrameForBackup = new xr::StereoRenderingFrame(true);

    // Register the command buffer request handler when creating the content renderer.
    content->setCommandBufferRequestHandler([this](TrCommandBufferBase *req)
                                            { this->onCommandBufferRequestReceived(req); });
  }

  TrContentRenderer::~TrContentRenderer()
  {
    content->resetCommandBufferRequestHandler();
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

  void TrContentRenderer::onCommandBufferRequestReceived(TrCommandBufferBase *req)
  {
    lock_guard<mutex> lock(commandBufferRequestsMutex);
    if (!req->renderingInfo.isValid() && !commandbuffers::isXRFrameControlCommandType(req->type))
    {
      defaultCommandBufferRequests.push_back(req);
    }
    else
    {
      int stereoId;
      int viewIndex;
      if (req->type == COMMAND_BUFFER_XRFRAME_START_REQ)
      {
        auto xrFrameStartReq = dynamic_cast<XRFrameStartCommandBufferRequest *>(req);
        stereoId = xrFrameStartReq->stereoId;
        viewIndex = xrFrameStartReq->viewIndex;
      }
      else if (req->type == COMMAND_BUFFER_XRFRAME_FLUSH_REQ)
      {
        auto xrFrameFlushReq = dynamic_cast<XRFrameFlushCommandBufferRequest *>(req);
        stereoId = xrFrameFlushReq->stereoId;
        viewIndex = xrFrameFlushReq->viewIndex;
      }
      else if (req->type == COMMAND_BUFFER_XRFRAME_END_REQ)
      {
        auto xrFrameEndReq = dynamic_cast<XRFrameEndCommandBufferRequest *>(req);
        stereoId = xrFrameEndReq->stereoId;
        viewIndex = xrFrameEndReq->viewIndex;
      }
      else
      {
        stereoId = req->renderingInfo.stereoId;
        viewIndex = req->renderingInfo.viewIndex;
      }

      for (auto frame : stereoFramesList)
      {
        if (frame->getId() == stereoId)
        {
          if (req->type == COMMAND_BUFFER_XRFRAME_START_REQ)
            frame->startFrame(viewIndex), delete req;
          else if (req->type == COMMAND_BUFFER_XRFRAME_FLUSH_REQ)
            frame->flushFrame(viewIndex), delete req;
          else if (req->type == COMMAND_BUFFER_XRFRAME_END_REQ)
            frame->endFrame(viewIndex), delete req;
          else
            frame->addCommandBuffer(req, viewIndex);
          break;
        }
      }
    }
  }

  void TrContentRenderer::onHostFrame(chrono::time_point<chrono::high_resolution_clock> time)
  {
    if (!shouldSkipDispatchingFrame(time))
    {
      bool isXRDeviceEnabled = xrDevice->enabled();
      if (isXRDeviceEnabled && xrDevice->isRenderedAsMultipass())
      {
        /**
         * As for normal animation frame request in multipass xr mode, we only need to dispatch it at left eye.
         */
        if (xrDevice->getActiveEyeId() == 0)
          dispatchAnimationFrameRequest();
      }
      else
      {
        dispatchAnimationFrameRequest();
      }

      if (isXRDeviceEnabled)
      {
        bool shouldDispatchXRFrame = false;
        xr::StereoRenderingFrame *stereoRenderingFrame = nullptr;
        // auto renderer = constellation->getRenderer();
        // auto rhi = renderer->getApi();

        if (xrDevice->isRenderedAsMultipass())
        {
          auto viewIndex = xrDevice->getActiveEyeId();
          stereoRenderingFrame = getOrCreateStereoFrame(xrDevice);
          if (stereoRenderingFrame != nullptr)
          {
            stereoRenderingFrame->available(true); // mark the StereoRenderingFrame is available

            xr::TrXRView view(viewIndex);
            auto viewport = xrDevice->getViewport(viewIndex);
            view.setViewport(viewport.width, viewport.height, viewport.x, viewport.y);
            view.setViewMatrix(xrDevice->getViewMatrixForEye(viewIndex));
            view.setProjectionMatrix(xrDevice->getProjectionMatrixForEye(viewIndex));

            if (viewIndex == 0) // Reset the `currentBaseXRFrameReq` when viewIndex is 0(left)
            {
              currentBaseXRFrameReq->reset();
              // Set `currentBaseXRFrameReq` with the related data.
              currentBaseXRFrameReq->stereoId = stereoRenderingFrame->getId();
              currentBaseXRFrameReq->setViewerBaseMatrix(xrDevice->getViewerBaseMatrix());
            }
            currentBaseXRFrameReq->viewIndex = viewIndex;
            currentBaseXRFrameReq->views[viewIndex] = view;
            shouldDispatchXRFrame = true;
          }
          else
          {
            // TODO: OOM handling?
          }
        }
        else if (xrDevice->getStereoRenderingMode() == xr::TrStereoRenderingMode::SinglePass)
        {
          // TODO: support SinglePass stereo rendering mode
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
    }

    onStartFrame();
    {
      executeCommandBuffers(false);
      if (xrDevice->enabled())
      {
        // set framebuffer?
        if (xrDevice->getStereoRenderingMode() == xr::TrStereoRenderingMode::MultiPass)
        {
          auto viewIndex = xrDevice->getActiveEyeId();
          executeCommandBuffers(true, viewIndex);
        }
      }
    }
    onEndFrame();
  }

  void TrContentRenderer::onStartFrame()
  {
    glContext->Restore();
    if (constellation->getRenderer()->isAppContextSummaryEnabled)
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

    auto hostContext = constellation->getRenderer()->glHostContext;
    auto hostViewport = hostContext->GetViewport();
    req.framebufferId = hostContext->GetFramebuffer();
    req.framebufferWidth = hostViewport.width;
    req.framebufferHeight = hostViewport.height;
    dispatchFrameRequest(req);
  }

  bool TrContentRenderer::shouldSkipDispatchingFrame(chrono::time_point<chrono::high_resolution_clock> time)
  {
    /**
     * If XR is disabled, frame skip is not allowed.
     */
    if (xrDevice == nullptr || !xrDevice->enabled())
      return false;

    /**
     * Zero `targetFrameRate` means disabling the frame rate control.
     */
    if (targetFrameRate == 0)
      return false;

    /**
     * Only if the XR Multipass (1 frame = 2 ticks), we will directly using when rendering right tick.
     */
    if (xrDevice->isRenderedAsMultipass() && xrDevice->getActiveEyeId() == 1)
      return skipDispatchingFrameState;

    /**
     * Main logic to check frame skipping.
     */
    if (isLastFrameTimepointSet == false)
    {
      lastFrameTimepoint = time;
      isLastFrameTimepointSet = true;
      skipDispatchingFrameState = false;
    }
    else
    {
      /**
       * We need to skip a frame based on the script frame rate to avoid the unnecessary CPU usage.
       */
      auto duration = chrono::duration_cast<chrono::milliseconds>(time - lastFrameTimepoint);
      if (duration.count() < 1000 / targetFrameRate)
      {
        skipDispatchingFrameState = true;
      }
      else
      {
        auto pendingFramesCount = getPendingStereoFramesCount();
        /**
         * When the frame count is greater than a fixed value, we can skip the frame for the script-side, namely in JavaScript, the
         * frame of this time will be dropped when the last frame is not finished.
         *
         * By using this method, we can avoid the frame is not rendered in time, but it will cause the frame rate in script is not
         * consistent with the host frame rate.
         */
        if (pendingFramesCount > 1)
          skipDispatchingFrameState = true;
        else
          skipDispatchingFrameState = false;
        lastFrameTimepoint = time;
      }
    }
    return skipDispatchingFrameState;
  }

  void TrContentRenderer::executeCommandBuffers(bool asXRFrame, int viewIndex)
  {
    if (content == nullptr) // FIXME: just skip executing command buffers if content is null, when content process is crashed.
      return;
    auto renderer = constellation->getRenderer();
    if (!asXRFrame)
    {
      lock_guard<mutex> lock(commandBufferRequestsMutex);
      auto commandBufferRequests = defaultCommandBufferRequests;
      renderer->executeCommandBuffers(commandBufferRequests, this);

      // clear the default commandbuffers queue.
      for (auto commandBufferReq : defaultCommandBufferRequests)
        delete commandBufferReq;
      defaultCommandBufferRequests.clear();
    }
    else
    {
      lock_guard<mutex> lock(commandBufferRequestsMutex);
      executeStereoFrame(viewIndex, [this](int stereoIdOfFrame, vector<TrCommandBufferBase *> &commandBufferRequests)
                         { return this->constellation->getRenderer()->executeCommandBuffers(commandBufferRequests, this); });
    }
  }

  bool TrContentRenderer::executeStereoFrame(int viewIndex, std::function<bool(int, std::vector<TrCommandBufferBase *> &)> exec)
  {
    bool called = false;
    for (auto it = stereoFramesList.begin(); it != stereoFramesList.end();)
    {
      auto frame = *it;
      if (
          !frame->available() /** Remove this frame when frame is still inavialble when executing */
          /**
           * We need to remove a frame when the frame is not started with a expiration check, and the expiration timeout is
           * configurable, its default value is 2000ms.
           *
           * TODO: make the expiration timeout to be configurable.
           */
          || (!frame->started() && frame->expired(2000)))
      {
        it = stereoFramesList.erase(it);
        delete frame;
        continue;
      }
      /** Just skip the non-ended frames. */
      if (!frame->ended())
      {
        /** Check there is a flush command buffers */
        if (frame->needFlush(viewIndex))
        {
          if (
              viewIndex == 0 ||
              (viewIndex == 1 && frame->ended(0)))
          {
            auto commandBuffers = frame->getCommandBuffers(viewIndex);
            exec(frame->getId(), commandBuffers);
            frame->clearCommandBuffers(viewIndex);
          }
        }
        it++;
        continue;
      }
      /** If an ended frame is empty, it's needed to be removed here. */
      if (frame->empty())
      {
        /**
         * Note: in C++ STL, the `erase` function will return the next iterator that we need to use instead of `it++`.
         */
        it = stereoFramesList.erase(it);
        delete frame;
        continue;
      }

      /**
       * When we are going to render right(1) eye, we can't render the frame which left frame is not finished.
       * Such as, the frame is ended before the native loop is going to render the right eye, thus the left eye
       * in this frame will be skipped.
       */
      if (viewIndex == 1 && !frame->finished(0))
      {
        it++;
        continue;
      }

      auto id = frame->getId();
      auto commandBuffers = frame->getCommandBuffers(viewIndex);
      auto isStateChanged = exec(id, commandBuffers);
      frame->idempotent(viewIndex, !isStateChanged);
      frame->finishPass(viewIndex);

      if (viewIndex == 1)
      {
        if (frame->idempotent())
          stereoFrameForBackup->copyCommandBuffers(frame);
        else
          stereoFrameForBackup->clearCommandBuffers();
      }

      /**
       * After rendering the right eye, we need to remove the frame.
       */
      if (viewIndex == 1)
      {
        assert(frame->finished(0));
        it = stereoFramesList.erase(it);
        delete frame;
      }
      else
      {
        it++;
      }

      /**
       * We only need to render the frame one by one, this avoids the rendering order is not correct.
       */
      called = true;
      break;
    }

    /**
     * When the `called` is false, it means the current frames are not ended, so we need to render by the last frame.
     */
    if (called == false)
    {
      auto &commandBufferInLastFrame = stereoFrameForBackup->getCommandBuffers(viewIndex);
      if (commandBufferInLastFrame.size() > 0)
        exec(stereoFrameForBackup->getId(), commandBufferInLastFrame);
    }
    return called;
  }

  xr::StereoRenderingFrame *TrContentRenderer::getOrCreateStereoFrame(xr::Device *xrDevice)
  {
    if (xrDevice->getActiveEyeId() == 0)
      stereoFramesList.push_back(xrDevice->createStereoRenderingFrame());
    return stereoFramesList.back();
  }

  size_t TrContentRenderer::getPendingStereoFramesCount()
  {
    lock_guard<mutex> lock(commandBufferRequestsMutex);
    size_t count = 0;
    for (auto frame : stereoFramesList)
    {
      if (frame->ended())
        count++;
    }
    return count;
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
