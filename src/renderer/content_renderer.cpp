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
        xrDevice(constellation->xrDevice.get()),
        targetFrameRate(constellation->renderer->clientDefaultFrameRate),
        currentBaseXRFrameReq(make_unique<xr::TrXRFrameRequest>())
  {
    assert(xrDevice != nullptr);
    glContext = new OpenGLAppContextStorage("content_renderer#" + std::to_string(content->pid));
    stereoFrameForBackup = new xr::StereoRenderingFrame(true);
    frameDispatcherThread = std::make_unique<WorkerThread>("FrameDispatcher", [this](WorkerThread &worker)
                                                           { this->onDispatchFrameRequest(worker); }, 1);

    // Register the command buffer request handler when creating the content renderer.
    content->setCommandBufferRequestHandler([this](TrCommandBufferBase *req)
                                            { this->onCommandBufferRequestReceived(req); });
  }

  TrContentRenderer::~TrContentRenderer()
  {
    frameDispatcherThread->stop();
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
    lastFrameHasOutOfMemoryError = false;
    lastFrameErrorsCount = 0;
    content->onCommandBuffersExecuting();
  }

  void TrContentRenderer::onCommandBuffersExecuted()
  {
    content->onCommandBuffersExecuted();
    if (lastFrameHasOutOfMemoryError || lastFrameErrorsCount > 10)
    {
      DEBUG(LOG_TAG_ERROR, "Disposing the content(%d) due to the frame OOM or occurred errors(%d) > 10",
            content->id, lastFrameErrorsCount);
      content->shouldDestroy = true;
    }
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

  void TrContentRenderer::onDispatchFrameRequest(WorkerThread &threadWorker)
  {
    auto now = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(now - lastDispatchedFrameTime).count();
    if (duration <= 1000 / targetFrameRate)
    {
      threadWorker.sleep();
      return;
    }
    lastDispatchedFrameTime = now;

    dispatchAnimationFrameRequest();
    if (xrDevice != nullptr && xrDevice->enabled())
    {
      if (
          isXRFrameBaseReqUpdating ||       // Skip the frame dispatching if the updating is not finished.
          isXRFrameBaseReqDirty == false || // Skip the frame dispatching if the base frame request is not dirty.
          getPendingStereoFramesCount() > 2)
        return;

      if (xrDevice->isRenderedAsMultipass()) // TODO: support singlepass?
      {
        shared_lock<shared_mutex> lock(mutexForXRFrameBaseReq);
        xrDevice->iterateSessionsByContentPid(content->pid, [this](xr::TrXRSession *session)
                                              {
                                                dispatchXRFrameRequest(session, 0);
                                                dispatchXRFrameRequest(session, 1); });
        isXRFrameBaseReqDirty.store(false);
      }
    }
  }

  void TrContentRenderer::onCommandBufferRequestReceived(TrCommandBufferBase *req)
  {
    if (!req->renderingInfo.isValid() && !commandbuffers::isXRFrameControlCommandType(req->type))
    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
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

      vector<xr::StereoRenderingFrame *> stereoFramesListCopy;
      {
        shared_lock<shared_mutex> lock(commandBufferRequestsMutex);
        stereoFramesListCopy = stereoFramesList;
      }
      for (auto frame : stereoFramesListCopy)
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
          {
            if (frame->ended())
            {
              DEBUG(LOG_TAG_ERROR, "The command buffer(%d) has been ignored due to the stereo frame(%d) is ended.",
                    req->type, stereoId);
              delete req;
            }
            else
            {
              unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
              frame->addCommandBuffer(req, viewIndex);
            }
          }
          break;
        }
      }
    }
  }

  void TrContentRenderer::onHostFrame(chrono::time_point<chrono::high_resolution_clock> time)
  {
    bool isXRDeviceEnabled = xrDevice->enabled();
    if (isXRDeviceEnabled)
    {
      if (xrDevice->isRenderedAsMultipass())
      {
        auto viewIndex = xrDevice->getActiveEyeId();
        auto stereoRenderingFrame = getOrCreateStereoFrame();
        if (stereoRenderingFrame != nullptr)
        {
          stereoRenderingFrame->available(true); // mark the StereoRenderingFrame is available

          xr::TrXRView view(viewIndex);
          auto viewport = xrDevice->getViewport(viewIndex);
          view.setViewport(viewport.width, viewport.height, viewport.x, viewport.y);
          view.setViewMatrix(xrDevice->getViewMatrixForEye(viewIndex));
          view.setProjectionMatrix(xrDevice->getProjectionMatrixForEye(viewIndex));
          {
            unique_lock<shared_mutex> lock(mutexForXRFrameBaseReq);
            if (viewIndex == 0) // Reset the `currentBaseXRFrameReq` when viewIndex is 0(left)
            {
              isXRFrameBaseReqUpdating.store(true);
              currentBaseXRFrameReq->reset();
              {
                // Set `currentBaseXRFrameReq` with the related data.
                currentBaseXRFrameReq->stereoId = stereoRenderingFrame->getId();
                currentBaseXRFrameReq->setViewerBaseMatrix(xrDevice->getViewerBaseMatrix());
              }
              isXRFrameBaseReqDirty.store(true);
            }
            else
              isXRFrameBaseReqUpdating.store(false);
            currentBaseXRFrameReq->views[viewIndex] = view;
          }
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
    if (constellation->renderer->isAppContextSummaryEnabled)
      glContext->Print();

    // Reset frame states
    drawCallsPerFrame = 0;
    drawCallsCountPerFrame = 0;
  }

  void TrContentRenderer::onEndFrame()
  {
  }

  inline void TrContentRenderer::dispatchAnimationFrameRequest()
  {
    TrAnimationFrameRequest req;
    dispatchFrameRequest(req);
  }

  inline void TrContentRenderer::dispatchXRFrameRequest(xr::TrXRSession *session, int viewIndex)
  {
    if (!session->isInFrustum())
      return;

    auto req = currentBaseXRFrameReq->clone();
    req.viewIndex = viewIndex;
    req.sessionId = session->id;
    req.setLocalBaseMatrix(session->getLocalBaseMatrix());

    auto hostContext = constellation->renderer->glHostContext;
    auto hostViewport = hostContext->GetViewport();
    req.framebufferId = hostContext->GetFramebuffer();
    req.framebufferWidth = hostViewport.width;
    req.framebufferHeight = hostViewport.height;
    dispatchFrameRequest(req);
  }

  void TrContentRenderer::executeCommandBuffers(bool asXRFrame, int viewIndex)
  {
    if (content == nullptr) // FIXME: just skip executing command buffers if content is null, when content process is crashed.
      return;

    if (!asXRFrame)
    {
      vector<commandbuffers::TrCommandBufferBase *> commandBufferRequests;
      {
        unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
        commandBufferRequests = defaultCommandBufferRequests;
        defaultCommandBufferRequests.clear();
      }
      constellation->renderer->executeCommandBuffers(commandBufferRequests, this);
      for (auto commandBufferReq : commandBufferRequests)
        delete commandBufferReq;
    }
    else
    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
      executeStereoFrame(viewIndex, [this](int stereoIdOfFrame, vector<TrCommandBufferBase *> &commandBufferRequests)
                         { return constellation->renderer->executeCommandBuffers(commandBufferRequests, this); });
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

  xr::StereoRenderingFrame *TrContentRenderer::getOrCreateStereoFrame()
  {
    unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
    if (xrDevice->getActiveEyeId() == 0)
      stereoFramesList.push_back(xrDevice->createStereoRenderingFrame());
    return stereoFramesList.back();
  }

  size_t TrContentRenderer::getPendingStereoFramesCount()
  {
    shared_lock<shared_mutex> lock(commandBufferRequestsMutex);
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
